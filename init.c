/////////////////////////////////////
//  Generated Initialization File  //
/////////////////////////////////////

#include "compiler_defs.h"
#include "C8051F580_defs.h"

// Peripheral specific initialization functions,
// Called from the Init_Device() function
void PCA0_Init()
{
    PCA0MD    &= ~0x40;
    PCA0MD    = 0x00;
}

void Port_IO_Init()
{
    // P0.0  -  Unassigned,  Open-Drain, Digital
    // P0.1  -  Unassigned,  Open-Drain, Digital
    // P0.2  -  Unassigned,  Open-Drain, Digital
    // P0.3  -  Unassigned,  Open-Drain, Digital
    // P0.4  -  Unassigned,  Open-Drain, Digital
    // P0.5  -  Unassigned,  Open-Drain, Digital
    // P0.6  -  Unassigned,  Open-Drain, Digital
    // P0.7  -  Unassigned,  Open-Drain, Digital

    // P1.0  -  Unassigned,  Open-Drain, Digital
    // P1.1  -  Unassigned,  Open-Drain, Digital
    // P1.2  -  Unassigned,  Open-Drain, Digital
    // P1.3  -  Unassigned,  Open-Drain, Digital
    // P1.4  -  Unassigned,  Open-Drain, Digital
    // P1.5  -  Unassigned,  Open-Drain, Digital
    // P1.6  -  Unassigned,  Open-Drain, Digital
    // P1.7  -  Unassigned,  Open-Drain, Digital

    // P2.0  -  Unassigned,  Open-Drain, Digital
    // P2.1  -  Unassigned,  Open-Drain, Digital
    // P2.2  -  Unassigned,  Open-Drain, Digital
    // P2.3  -  Unassigned,  Open-Drain, Digital
    // P2.4  -  Unassigned,  Open-Drain, Digital
    // P2.5  -  Unassigned,  Open-Drain, Digital
    // P2.6  -  Unassigned,  Open-Drain, Digital
    // P2.7  -  Unassigned,  Open-Drain, Digital

    // P3.0  -  Unassigned,  Open-Drain, Digital
    // P3.1  -  Unassigned,  Open-Drain, Digital
    // P3.2  -  Unassigned,  Open-Drain, Digital
    // P3.3  -  Unassigned,  Open-Drain, Digital
    // P3.4  -  Unassigned,  Open-Drain, Digital
    // P3.5  -  Unassigned,  Open-Drain, Digital
    // P3.6  -  Unassigned,  Open-Drain, Digital
    // P3.7  -  Unassigned,  Open-Drain, Digital

    // P4.0  -  Unassigned,  Open-Drain, Digital [Fixed]
    // P4.1  -  Unassigned,  Open-Drain, Digital [Fixed]
    // P4.2  -  Unassigned,  Open-Drain, Digital [Fixed]
    // P4.3  -  Unassigned,  Open-Drain, Digital [Fixed]
    // P4.4  -  Unassigned,  Open-Drain, Digital [Fixed]
    // P4.5  -  Unassigned,  Open-Drain, Digital [Fixed]
    // P4.6  -  Unassigned,  Open-Drain, Digital [Fixed]
    // P4.7  -  Unassigned,  Open-Drain, Digital [Fixed]

    SFRPAGE   = CONFIG_PAGE;
    XBR2      = 0x40;
    SFRPAGE   = ACTIVE_PAGE;
}

void Oscillator_Init()
{
    SFRPAGE   = CONFIG_PAGE;
    OSCICN    = 0xC7;
    SFRPAGE   = ACTIVE_PAGE;
}

// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
    PCA0_Init();
    Port_IO_Init();
    Oscillator_Init();
}
