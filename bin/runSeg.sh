path="input/"
downsample="1"

## use palm-leaf image
inputfile="palmlf.jpg"
FGScribFile="palmlf.fg.1.dat"
BGScribFile="palmlf.bg.1.dat"

## use penguin image
# inputfile="pg1.jpg"
# FGScribFile="pg1.fg.1.dat"
# BGScribFile="pg1.bg.1.dat"

## use cow image
# inputfile="cow.png"
# FGScribFile="cow.fg.1.dat"
# BGScribFile="cow.bg.1.dat"

## use manga image
# inputfile="manga1.jpg"
# FGScribFile="manga1.fg.1.dat"
# BGScribFile="manga1.bg.1.dat"

# number of dimensions of embedding
# low dimensionality (3-6) works well
dim="5"

# Nystrom sampling rate
# NysSampling=0.01 => approximately (0.01 * 0.01 *  TotalNPixels) samples
NysSampling=0.01

# Dissimilarity
# Tested choices are: {EUC, BIL, FAB, PAT, L1, CHISQ, EMD, DMAP, BIH}
# Recommended: EUC and CHISQ
 distMetric="EUC"
UseLUV=0

# distMetric="CHISQ"
# UseLUV=1

# parameters for dissimilarity (only used for BIL and FAB)
sigmar=.5
sigmas=5

# Patch size
# Only applies for CHISQ, PAT, EMD, FAB
PS="1"

# Parameters to [KK11] for MAP inference in fully connected CRF
# a) when multiple values are defined, eg. by setting
#sigcrf="--crf_sigma .1 --crf_sigma 1 --crf_sigma 10"
# the inference is performed for 
# all combinations of the two input parameters sigma and w
# b) wcrf should not need to be changed
# c) sigcrf of .1, 1 or 10 works best depending on distance metric
sigcrf="--crf_sigma .1"
wcrf="--crf_weight 20"


outfile="./output/$inputfile.$dim.$distMetric.$NysSampling.$UseLUV"

###############################
# Calling the executable RobustSelect
###############################
./RobustSelect -v $UseLUV \
  -x $downsample \
    -d $dim  \
    -p $path \
    -f $inputfile \
    -D $distMetric \
    -n $NysSampling \
    --patch_size $PS \
    -r $sigmar -s $sigmas \
    -C $FGScribFile \
    -C $BGScribFile \
    -C $outfile \
    $sigcrf $wcrf
