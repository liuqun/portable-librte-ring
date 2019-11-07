TEMPLATE = subdirs

SUBDIRS += \
        TestBasicMutiProducer \
        TestMPMC \
        librte_ring

TestMPMC.depends = librte_ring
TestBasicMutiProducer.depends = librte_ring
