minisat.d: ../../include/Solver.h ../../include/Proof.h ../../include/SolverTypes.h ../../include/VarOrder.h ../../include/File.h ../../include/Global.h ../../include/Heap.h ../../include/Sort.h 
../../include/Solver.h: Solver.h
	@rm -f ../../include/Solver.h
	@ln -fs ../src/minisat/Solver.h ../../include/Solver.h
../../include/Proof.h: Proof.h
	@rm -f ../../include/Proof.h
	@ln -fs ../src/minisat/Proof.h ../../include/Proof.h
../../include/SolverTypes.h: SolverTypes.h
	@rm -f ../../include/SolverTypes.h
	@ln -fs ../src/minisat/SolverTypes.h ../../include/SolverTypes.h
../../include/VarOrder.h: VarOrder.h
	@rm -f ../../include/VarOrder.h
	@ln -fs ../src/minisat/VarOrder.h ../../include/VarOrder.h
../../include/File.h: File.h
	@rm -f ../../include/File.h
	@ln -fs ../src/minisat/File.h ../../include/File.h
../../include/Global.h: Global.h
	@rm -f ../../include/Global.h
	@ln -fs ../src/minisat/Global.h ../../include/Global.h
../../include/Heap.h: Heap.h
	@rm -f ../../include/Heap.h
	@ln -fs ../src/minisat/Heap.h ../../include/Heap.h
../../include/Sort.h: Sort.h
	@rm -f ../../include/Sort.h
	@ln -fs ../src/minisat/Sort.h ../../include/Sort.h
