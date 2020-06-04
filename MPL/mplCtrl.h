#ifndef MPL_CTRL_H
#define MPL_CTRL_H

// MPL_FN
// Declared at top of a function.
#define MPL_BEGIN_FUNC mpl::wide_bool mask = true

// MPL_IF
// NOTE: Macro contains necessary braces
#define MPL_IF(condition) \
	{ \
	const mpl::wide_bool &malias = &mask; \
	mpl::wide_bool mask = (condition) & malias; \
	if (!mask.all_fail()) {

#define MPL_ELIF(condition) \
	} \
	mask = (condition) & mask; \
	if (!mask.all_fail()) {

#define MPL_ELSE \
	} \
	mask = !mask; \
	if (!mask.all_fail()) {

#define MPL_ENDIF }}

#define MPL_OUT(l, r) \
	mpl::cmov(mask, l, r)

#define MPL_WHILE(condition) \
	{ \
	const mpl::wide_bool &malias = &mask; \
	mpl::wide_bool mask = (condition) & malias; \
	while (!mask.all_fail()) {

#define MPL_ENDWHILE }}

#endif // MPL_CTRL_H
