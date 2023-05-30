/*
 * Name				:	util.cpp
 * Created			:	5/25/2022 3:15:43 PM
 * Author			:	Andrew Bennett
 * Description		:	A utility library containing various functions for the ATSAMD21 microcontroller family.
 */

#include "util.h"

uint32_t Util::critical_section_count = 0;

void Util::enterCriticalSection()
{
    if(Util::critical_section_count == 0)
    {
        __disable_irq();
    }
    Util::critical_section_count++;
}

void Util::exitCriticalSection()
{
    Util::critical_section_count--;

    if(Util::critical_section_count == 0)
    {
        __enable_irq();
    }
}
