# Light-Recursive-SBDS-with-incNGs
Light Recursive SBDS with incNGs global constraint

This is the implementation of the method LReSBDS using the incNGs global constraint proposed in the paper "Jimmy Lee and Zichen Zhu. An Increasing-Nogoods Global Constraint for Symmetry Breaking During Search, Proceedings of the 20th International Conference on Principles and Practice of Constraint Programming (CP 2014), pages 465-480, Lyon, France, September, 2014"

Please use Gecode Solver 4.2.0 to run these files.

Put incNGs folder into gecode folder and queens_lresbds_incNGs.cpp file into example folder.

To run the N-Queens problem (e.g. N=10) using one of the symmetry breaking methods SBDS/ParSBDS/LReSBDS/LDSB:

./queens_lresbds_incNGs -search sbds 10

./queens_lresbds_incNGs -search parsbds 10

./queens_lresbds_incNGs -search lresbds 10

./queens_lresbds_incNGs -search ldsb 10
