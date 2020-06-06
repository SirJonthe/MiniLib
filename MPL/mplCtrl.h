#ifndef MPL_CTRL_H
#define MPL_CTRL_H

// MPL_FN
// Declared at top of a function.
#define MPL_BEGIN_FUNC mpl::wide_bool mask = true

// MPL_IF
// Marks the start of a true condition in a conditional block.
// NOTE: Macro contains necessary braces.
#define MPL_IF(condition) \
	{ \
	const mpl::wide_bool &mask0 = mask; \
	mpl::wide_bool mask = (condition) & mask0; \
	if (!mask.all_fail()) {

//#define MPL_ELSEIF(condition) \
//	} \
//	mask = (condition) & !mask & mask0; \
//	if (!mask.all_fail()) {

//#define MPL_ELIF(condition) MPL_ELSEIF(condition)

// MPL_ELSE
// Marks the start of the false condition in a conditional block.
// NOTE: Macro contains necessary braces.
#define MPL_ELSE \
	} \
	mask = !mask & mask0; \
	if (!mask.all_fail()) {

// MPL_ENDIF
// Denotes the end of a conditional block.
#define MPL_ENDIF }}

// MPL_SET
// Used to assign values to variables not declared inside the current stack frame.
// Returns L if previous test was TRUE, or else returns nothing.
#define MPL_SET(l) \
	mpl::cset<decltype(l)>{ mask, l }

// MPL_WHILE
// Marks the start of a while conditional loop.
// NOTE: Macro contains necessary braces.
#define MPL_WHILE(condition) \
	{ \
	const mpl::wide_bool &mask0 = &mask; \
	mpl::wide_bool mask = (condition) & mask0; \
	while (!mask.all_fail()) {

// MPL_ENDWHILE
// Marks the end of a while conditional loop.
#define MPL_ENDWHILE }}

#endif // MPL_CTRL_H
