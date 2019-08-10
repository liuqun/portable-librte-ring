TEMPLATE = subdirs

SUBDIRS += \
        TestMPMC \
        librte_ring

TestMPMC.depends = librte_ring
