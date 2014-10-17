#include "MTL/mtlBinaryTree.h"
#include "MTL/mtlParser.h"
#include "MTL/mtlExpression.h"

#include "MGL/mglCamera.h"
#include "MGL/mglFramebuffer.h"
#include "MGL/mglRasterizer.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

void Unit_StringAppend( void );
void Unit_StringOverwrite( void );
void Unit_Clipping( void );
void Unit_Directory( void );
void Unit_QuatToMatrix( void );
void Unit_MergeLists( void );
void Unit_BinaryTree( void );
void Unit_Expression( void );
void Unit_MathExpression( void );

std::ostream &operator<<(std::ostream &out, const mtlString &str)
{
	return out << str.GetChars();
}

std::ostream &operator<<(std::ostream &out, const mtlChars &str)
{
	for (int i = 0; i < str.GetSize(); ++i) {
		out << str.GetChars()[i];
	}
	return out;
}

// TODO: Implement SIMD parallel rasterizer, render in blocks of X pixels
// TODO: Implement CPU parallel rasterizer, one CPU per SIMD block scanline (local cache coherency)
	// std::thread or OpenMP?
	// http://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/
// TODO: Implement fixed point rasterizer (fstp seems expensive), may not be compatible with SIMD
// TODO: group together the least important lights into a directional light
// TODO: transform lights to object space, light model (prevents having to transform model normals)
// TODO: stop splitting BSP when we get a fully convex subspace (possible within reasonable time consumption?)
// TODO: Transform with constraints (e.g. can't move along Z, can't rotate around Y and so on)
// TODO: Refactor renderer (completely generic rendering design), only provides interface for rendering primitives
// TODO: Model renders itself, uses renderer interface - PROBLEM: how to render stencil shadows? Separate rendering function?
// TODO: Texture and Model need virtual API interface functions
// TODO: Fix mtlString::Append()

// BUG: mtlString::Insert does not work when appending, does it work for other cases?
// BUG: mtlString::Overwrite fails to write a larger string than is already available
// BUG: Multiple materials in a material file crashes model loader (maybe even multiple materials regardless of separate files?)
// BUG: Trailing newline may prevent or cause a failed read in model/material loader
// BUG: BSP models are sometimes missing polygons
	// could these polygons be the splitting polygons?
// BUG: BSP does not always work when very close to geometry or viewed from very oblique angles
// BUG: rotating by global axis might actually be rotating by local axis
// BUG: mouse down event generates uninitialized value somewhere
// BUG: preservation of transforms does not work

// http://devmaster.net/posts/11968/cracks-in-geometry-in-software-rasterizer
// http://chrishecker.com/Miscellaneous_Technical_Articles

int main(int argc, char **argv)
{
	std::cout << "Unit tests:" << std::endl;
	Unit_Clipping();
	Unit_StringAppend();
	Unit_StringOverwrite();
	Unit_Directory();
	Unit_QuatToMatrix();
	Unit_MergeLists();
	Unit_BinaryTree();
	Unit_Expression();
	Unit_MathExpression();
	std::cout << "completed" << std::endl;

	return 0;
}

void Unit_StringAppend( void )
{
	std::cout << "\tTesting string append...";
	mtlString str1;
	str1.Copy("Hello");
	mtlString str2;
	str2.Copy(", World");
	mtlString str3;
	str3.Copy(str1);
	str3.Append(str2);
	if (str3.GetSize() == str1.GetSize() + str2.GetSize()) {
		std::cout << str3 << std::endl;
	} else {
		std::cout << "failed: " << str3.GetSize() << " vs " << str1.GetSize() << "+" << str2.GetSize() << std::endl;
	}
}

void Unit_StringOverwrite( void )
{
	std::cout << "\tTesting string overwrite...";
	mtlString str1;
	str1.Copy("Hello");
	mtlString str2;
	str2.Copy("J");
	mtlString str3;
	str3.Copy(str1);
	str3.Overwrite(str2, 0);
	mtlString str4;
	str4.Copy(str1);
	str4.Overwrite("A big write", 0);
	if (str3.Compare("Jello") && str4.Compare("A big write")) {
		std::cout << "success" << std::endl;
	} else {
		std::cout << "failed: " << str3 << ", " << str4 << std::endl;
	}
}

void Unit_Clipping( void )
{
	mmlVector<3> a(0.0f,  50.0f,  50.0f);
	mmlVector<3> b(0.0f, -50.0f,  50.0f);
	mmlVector<3> c(0.0f,   0.0f, -50.0f);
	mmlVector<3> out1[4];
	mmlVector<3> out2[4];
	mglPlane plane(mmlVector<3>(0.0f, 0.0f, 0.0f), mglTransform::globalAxis.forward);
	plane.Clip(a, b, c, out1);
	std::cout << "\tTesting plane edge case...";
	if (plane.Clip(out1[0], out1[1], out1[2], out2) == 3 && plane.Clip(out1[0], out1[2], out1[3], out2) == 3) {
		plane.ReverseClip(a, b, c, out1);
		if (plane.Clip(out1[0], out1[1], out1[2], out2) == 0 && plane.Clip(out1[0], out1[2], out1[3], out2) == 0) {
			std::cout << "success" << std::endl;
		} else {
			std::cout << "failed" << std::endl;
		}
	} else {
		std::cout << "failed" << std::endl;
	}
}

void Unit_Directory( void )
{
	mtlDirectory dir1("~/Applications/MiniLib.app");
	mtlDirectory dir2("~/Applications/MiniLib.app/");
	mtlDirectory dir3("~/Applications/MiniLib.app/Contents/MacOS/MiniLib");
	mtlDirectory dir4("~/Applications/MiniLib.app/Contents/MacOS/MiniLib.ub");
	mtlDirectory dir5("~/Applications/MiniLib.app/.config.ini");
	mtlDirectory dir6("~/Applications/MiniLib.app/.config");
	mtlDirectory dir7("MiniLib.app");
	mtlDirectory dir8("/MiniLib.app");
	mtlDirectory dir9(".config");
	mtlDirectory dir10(".config.ini");
	
	std::cout << "\tTesting mtlDirectory..." << std::endl;
	
	if (!dir1.GetFolders().Compare("~/Applications/") || !dir1.GetFilename().Compare("MiniLib") || !dir1.GetExtension().Compare("app")) {
		std::cout << "\t\t" << dir1.GetDirectory() << ": failed 1" << std::endl;
		std::cout << "\t\t\t" << dir1.GetFolders() << " | " << dir1.GetFilename() << " | " << dir1.GetExtension() << std::endl;
	}
	if (!dir2.GetFolders().Compare("~/Applications/MiniLib.app/") || !dir2.GetFilename().Compare("") || !dir2.GetExtension().Compare("")) {
		std::cout << "\t\t" << dir2.GetDirectory() << ": failed 2" << std::endl;
		std::cout << "\t\t\t" << dir2.GetFolders() << " | " << dir2.GetFilename() << " | " << dir2.GetExtension() << std::endl;
	}
	if (!dir3.GetFolders().Compare("~/Applications/MiniLib.app/Contents/MacOS/") || !dir3.GetFilename().Compare("MiniLib") || !dir3.GetExtension().Compare("")) {
		std::cout << "\t\t" << dir3.GetDirectory() << ": failed 3" << std::endl;
		std::cout << "\t\t\t" << dir3.GetFolders() << " | " << dir3.GetFilename() << " | " << dir3.GetExtension() << std::endl;
	}
	if (!dir4.GetFolders().Compare("~/Applications/MiniLib.app/Contents/MacOS/") || !dir4.GetFilename().Compare("MiniLib") || !dir4.GetExtension().Compare("ub")) {
		std::cout << "\t\t" << dir4.GetDirectory() << ": failed 4" << std::endl;
		std::cout << "\t\t\t" << dir4.GetFolders() << " | " << dir4.GetFilename() << " | " << dir4.GetExtension() << std::endl;
	}
	if (!dir5.GetFolders().Compare("~/Applications/MiniLib.app/") || !dir5.GetFilename().Compare(".config") || !dir5.GetExtension().Compare("ini")) {
		std::cout << "\t\t" << dir5.GetDirectory() << ": failed 5" << std::endl;
		std::cout << "\t\t\t" << dir5.GetFolders() << " | " << dir5.GetFilename() << " | " << dir5.GetExtension() << std::endl;
	}
	if (!dir6.GetFolders().Compare("~/Applications/MiniLib.app/") || !dir6.GetFilename().Compare(".config") || !dir6.GetExtension().Compare("")) {
		std::cout << "\t\t" << dir6.GetDirectory() << ": failed 6" << std::endl;
		std::cout << "\t\t\t" << dir6.GetFolders() << " | " << dir6.GetFilename() << " | " << dir6.GetExtension() << std::endl;
	}
	if (!dir7.GetFolders().Compare("") || !dir7.GetFilename().Compare("MiniLib") || !dir7.GetExtension().Compare("app")) {
		std::cout << "\t\t" << dir7.GetDirectory() << ": failed 7" << std::endl;
		std::cout << "\t\t\t" << dir7.GetFolders() << " | " << dir7.GetFilename() << " | " << dir7.GetExtension() << std::endl;
	}
	if (!dir8.GetFolders().Compare("/") || !dir8.GetFilename().Compare("MiniLib") || !dir8.GetExtension().Compare("app")) {
		std::cout << "\t\t" << dir8.GetDirectory() << ": failed 8" << std::endl;
		std::cout << "\t\t\t" << dir8.GetFolders() << " | " << dir8.GetFilename() << " | " << dir8.GetExtension() << std::endl;
	}
	if (!dir9.GetFolders().Compare("") || !dir9.GetFilename().Compare(".config") || !dir9.GetExtension().Compare("")) {
		std::cout << "\t\t" << dir9.GetDirectory() << ": failed 9" << std::endl;
		std::cout << "\t\t\t" << dir9.GetFolders() << " | " << dir9.GetFilename() << " | " << dir9.GetExtension() << std::endl;
	}
	if (!dir10.GetFolders().Compare("") || !dir10.GetFilename().Compare(".config") || !dir10.GetExtension().Compare("ini")) {
		std::cout << "\t\t" << dir10.GetDirectory() << ": failed 10" << std::endl;
		std::cout << "\t\t\t" << dir10.GetFolders() << " | " << dir10.GetFilename() << " | " << dir10.GetExtension() << std::endl;
	}
	
	std::cout << "\t\tdone" << std::endl;
}

void Unit_QuatToMatrix( void )
{
	std::cout << "\tTesting quaternion...";
	mglTransform transform;
	transform.position = mmlVector<3>(40.0f, 14.0f, 73.0f);
	transform.rotation.RotateByAxisAngle(mglTransform::globalAxis.up, 14.3f);
	transform.rotation.RotateByAxisAngle(mglTransform::globalAxis.right, 29.2f);
	transform.rotation.RotateByAxisAngle(mglTransform::globalAxis.forward, 1.7f);
	mmlMatrix<4,4> matrix = transform.GetWorldTransformMatrix();
	
	mmlVector<3> pointT = transform.position;
	mmlVector<3> pointM = mmlVector<3>(0.0f, 0.0f, 0.0f) * matrix;
	
	std::cout << "t(" << pointT[0] << ";" << pointT[1] << ";" << pointT[2] << ") : m(" << pointM[0] << ";" << pointM[1] << ";" << pointM[2] << ")" << std::endl;
}

void Unit_MergeLists( void )
{
	mtlList<int> l1;
	l1.AddLast(3);
	l1.AddLast(7);
	l1.AddLast(9);
	l1.AddLast(19);
	l1.AddLast(67);
	
	mtlList<int> l2;
	l2.AddLast(1);
	l2.AddLast(2);
	l2.AddLast(4);
	l2.AddLast(8);
	l2.AddLast(10);
	l2.AddLast(77);
	
	mtlList<int> l3;
	
	mtlNode<int> *n1 = l1.GetFirst();
	mtlNode<int> *n2 = l2.GetFirst();
	
	while (true) { // assumes neither lists return NULL for a start
		
		if (n1->GetItem() < n2->GetItem()) {
			l3.AddFirst(n1->GetItem());
			n1 = n1->GetNext();
			if (n1 == NULL) {
				while (n2 != NULL) {
					l3.AddFirst(n2->GetItem());
					n2 = n2->GetNext();
				}
				return;
			}
		} else {
			l3.AddFirst(n2->GetItem());
			n2 = n2->GetNext();
			if (n2 == NULL) {
				while (n1 != NULL) {
					l3.AddFirst(n1->GetItem());
					n1 = n1->GetNext();
				}
				return;
			}
		}
	}
	
	mtlNode<int> *n3 = l3.GetFirst();
	while (n3 != NULL) {
		std::cout << n3->GetItem() << " ";
		n3 = n3->GetNext();
	}
}

void Unit_BinaryTree( void )
{
	std::cout << "\tTesting binary tree...";
	
	mtlBinaryTree<int> tree;
	
	for (int i = 0; i < 10; ++i) {
		int item = rand()%10;
		tree.Insert(item);
		std::cout << item << " ";
	}
	
	int find = rand()%10;
	const mtlBranch<int> *n = tree.GetRoot()->Find(find);
	if (n == NULL) { std::cout << ": (" << find << ")"; }
	else { std::cout << ": [" << find << "]"; }
	std::cout << std::endl;
}

void Unit_Expression( void )
{
	std::cout << "\tTesting expression matching..." << std::endl;

	mtlString buffer;
	buffer.Copy("func1 =   { 10, a string, 123.0f } \n\tfunc2 = { [1;2; 3], word }\nsome string = \"  this is a complete string \"\t\n");

	mtlParser parser;
	parser.SetBuffer(buffer);

	mtlList<mtlChars> output;

	mtlParser::ExpressionResult result = parser.Match("%s={%i,%s,%f}%n", output);
	if (result != mtlParser::ExpressionFound) {
		std::cout << "\t\tMismatch (1): " << result << std::endl;
		return;
	} else {
		std::cout << "\t\tExpression 1: ";
		mtlNode<mtlChars> *n = output.GetFirst();
		while (n != NULL) {
			std::cout << "\"" << n->GetItem() << "\" ";
			n = n->GetNext();
		}
		std::cout << std::endl;
	}

	result = parser.Match("%s={[%i;%i;%i],%w}%n", output);
	if (result != mtlParser::ExpressionFound) {
		std::cout << "\t\tMismatch (2): " << result << std::endl;
		return;
	} else {
		std::cout << "\t\tExpression 2: ";
		mtlNode<mtlChars> *n = output.GetFirst();
		while (n != NULL) {
			std::cout << "\"" << n->GetItem() << "\" ";
			n = n->GetNext();
		}
		std::cout << std::endl;
	}

	result = parser.Match("%s=\"%s\"%n", output);
	if (result != mtlParser::ExpressionFound) {
		std::cout << "\t\tMismatch (3): " << result << std::endl;
		return;
	} else {
		std::cout << "\t\tExpression 3: ";
		mtlNode<mtlChars> *n = output.GetFirst();
		while (n != NULL) {
			std::cout << "\"" << n->GetItem() << "\" ";
			n = n->GetNext();
		}
		std::cout << std::endl;
	}

	std::cout << "\t\tsuccess!" << std::endl;
	return;
}

void Unit_MathExpression( void )
{
	std::cout << "Unit_MathExpression...";
	mtlExpression expr;
	expr.SetConstant("Pi", 3.14159f);
	expr.SetExpression("(0+1)*Pi");
	std::cout << expr.GetExpression() << " = " << expr.Evaluate() << std::endl;
}
