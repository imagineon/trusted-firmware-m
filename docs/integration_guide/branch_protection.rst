#################
Branch Protection
#################

The Branch Protection is an optional feature that can help with mitigation of
common exploits through undesired branching in code.

The Branch Protection feature leverages the Pointer Authentication and Branch
Target Identification extension (PACBTI) which is optional and available in
Armv8.1-M architecture.

PACBTI is designed to mitigate Return-Oriented Programming (ROP) and
Jump-Oriented Programming (JOP) security exploit attacks.


Pointer authentication
======================

Pointer authentication (PAC) is a technique by which the pointer is "signed".
This signed pointer is generated by the hardware by combining a modifier, a
secret key and the pointer itself through a hardware cryptographic mechanism.

The generated signed pointer is produced at the beginning of a subroutine or
function.
On returning from the subroutine/function another signed pointer is generated
and compared with the previous signed pointer. If they match, then the pointer
(return address) has not been tampered and there are no side-effects.
Otherwise the validating instruction generates a UsageFault exception.

All the above functionality is achieved by introducing specific instructions at
the beginning and at the end of the subroutine/function. This addition is
controlled by compiler options, when supported.

To enable PAC in your platform, TF-M supports the following build options:

  - BRANCH_PROTECTION_PACRET
  - BRANCH_PROTECTION_PACRET_LEAF

One of the above values can be assigned to ``CONFIG_TFM_BRANCH_PROTECTION_FEAT``

``BRANCH_PROTECTION_PACRET_LEAF`` extends ``BRANCH_PROTECTION_PACRET`` when PAC
is desired also in leaf functions.


Branch target identification
============================

Branch Target Identification (BTI) can mitigate some of the JOP attacks so that
indirect branches can only allow target instructions. Such instructions are also
called landing pads.
In other words, an indirect branch can only "land" where a specific instruction
is present, otherwise the branch has been compromised and a UsageFault exception
is raised.

The above functionality is achieved by introducing specific instructions at the
beginning of executable code.

To enable BTI in your platform, TF-M supports the following build option:

  - BRANCH_PROTECTION_BTI

One of the above values can be assigned to ``CONFIG_TFM_BRANCH_PROTECTION_FEAT``


PAC and BTI
===========

It is possible to have some combinations of the PAC and BTI options above.
The allowed values for ``CONFIG_TFM_BRANCH_PROTECTION_FEAT`` are listed below,
but only one option can be used:

* ``BRANCH_PROTECTION_DISABLED`` - PACBTI feature is disabled
* ``BRANCH_PROTECTION_NONE`` - PACBTI instructions are not generated
* ``BRANCH_PROTECTION_STANDARD`` - Enables pointer authentication and branch target identification, leaf functions excluded
* ``BRANCH_PROTECTION_PACRET`` - Enables pointer authentication only, leaf functions excluded
* ``BRANCH_PROTECTION_PACRET_LEAF`` - Enables pointer authentication on all functions, including leaf functions
* ``BRANCH_PROTECTION_BTI`` - Enables branch target identification only

Note that PACBTI is conditional to both architecture support and compiler
support.

The build system performs checks to ensure that support is available and
terminates the build process if the required conditions are not met.


Limitations
===========

Currently PACBTI is supported only for Arm Compiler.
Even though GNUARM supports PACBTI, some features are missing that prevent all
the component integration to successfully work together.
It may be possible in the future to extend support for other compilers.

--------------

For further details on PACBTI features refer to:

`Armv8-M Architecture Reference Manual <https://developer.arm.com/documentation/ddi0553/latest>`_

`Armv8.1-M Pointer Authentication and Branch Target Identification Extension <https://community.arm.com/arm-community-blogs/b/architectures-and-processors-blog/posts/armv8-1-m-pointer-authentication-and-branch-target-identification-extension>`_

--------------

*Copyright (c) 2024, Arm Limited. All rights reserved.*
