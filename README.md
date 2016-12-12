# TagIt
Tag(ging) It(erative) of SNVs in multiple populations.

## Setup
Download latest version of TagIt source code from GitHub and run **make** command in *src* directory. If compilation is successful, **tagit** executable will be created in *src* directory. 

## Usage Example
To have a full list of supported tagging parameters and their description, execute:
```
./tagit --help
```
The minmal required data for tagging are: variant frequencies for each population, precomputed linkage-disequilibrium (rsquare) between variants for each population. The example data is located in *Data* directory. To run tagging using example data, execute:
```
./tagit --af ../Data/AFR.chr20.phase1_release_v3.20101123.freq ../Data/EUR.chr20.phase1_release_v3.20101123.freq --ld ../Data/AFR.chr20.phase1_release_v3.20101123.r2_0.5.pairLD.txt.gz ../Data/EUR.chr20.phase1_release_v3.20101123.r2_0.5.pairLD.txt.gz --r2 0.3 --out-summary summary.txt.gz --out-tagged tagged.txt.gz --out-tags tags.txt.gz
```
