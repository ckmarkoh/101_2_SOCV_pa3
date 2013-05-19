vrf.d: ../../include/v3VrfBase.h ../../include/v3VrfCmd.h ../../include/v3VrfResult.h ../../include/v3VrfSIM.h ../../include/v3VrfUMC.h 
../../include/v3VrfBase.h: v3VrfBase.h
	@rm -f ../../include/v3VrfBase.h
	@ln -fs ../src/vrf/v3VrfBase.h ../../include/v3VrfBase.h
../../include/v3VrfCmd.h: v3VrfCmd.h
	@rm -f ../../include/v3VrfCmd.h
	@ln -fs ../src/vrf/v3VrfCmd.h ../../include/v3VrfCmd.h
../../include/v3VrfResult.h: v3VrfResult.h
	@rm -f ../../include/v3VrfResult.h
	@ln -fs ../src/vrf/v3VrfResult.h ../../include/v3VrfResult.h
../../include/v3VrfSIM.h: v3VrfSIM.h
	@rm -f ../../include/v3VrfSIM.h
	@ln -fs ../src/vrf/v3VrfSIM.h ../../include/v3VrfSIM.h
../../include/v3VrfUMC.h: v3VrfUMC.h
	@rm -f ../../include/v3VrfUMC.h
	@ln -fs ../src/vrf/v3VrfUMC.h ../../include/v3VrfUMC.h
