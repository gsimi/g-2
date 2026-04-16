# Trigger discriminator configuration

# main executable
cd /home/lhcb/g-2/daq/TriggerDiscr
emacs discrConfig.c
make -f Makefile.discr discrConfig
./discrConfig

# old versions
cd /home/lhcb/g-2/daq/TriggerDiscr/old_versions
# example:
make -f Makefile.vmedemoEff discrConfigEff
