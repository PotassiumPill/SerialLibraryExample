/*
 * Name				:	util.h
 * Created			:	5/25/2022 12:25:43 PM
 * Author			:	Andrew Bennett
 * Description		:	A utility library containing various functions for the ATSAMD21 microcontroller family.
 */

#ifndef UTIL_H
#define UTIL_H

#include "sam.h"

/*!
 * \brief Utility library global namespace.
 *
 * This namespace contains all objects and global definitions used by the utility library. These objects and globals are used
 * to perform various utility functions.
 */
namespace Util
{
    /*!
     * \brief Enters a critical section.
     *
     * Declares the beginning of a critical section and disables interrupts.
     */
    void enterCriticalSection();

    /*!
     * \brief Exits a critical section.
     * 
     * Declares the end of a critical section and re-enables interrupts if there are no nested critical sections currently being
     * handled.
     */
    void exitCriticalSection();
	/*!
     * \brief Keeps track of nested critical sections. Each additional entry into a critical section increments by 1 and each
     *        exit decrements by 1. If it reaches zero then all nested critical sections have been exited and interrupts are
     *        re-enabled. Should not be modified by the user.
     */
	extern uint32_t critical_section_count;
}

#endif
