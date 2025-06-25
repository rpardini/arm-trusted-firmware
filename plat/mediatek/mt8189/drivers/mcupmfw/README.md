
# MCUPM introduction
MCUPM is a hardware module which is used for MCUSYS Power Management.
MCUPM firmware (`mcupm.bin`) is loaded into MCUPM SRAM at system initialization.

## Who uses it
ATF will load MCUPM at bl31 stage. It will copy mcupm.bin to MCUPM SRAM.

## How to load `mcupm.bin`
Copy `mcupm.bin` to MCUPM SRAM, then set normal boot flag and release software reset pin of MCUPM.

## Return values
No return value.

## Version
`$ strings mcupm.bin | grep "MCUPM firmware"`
