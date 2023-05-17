from . import ipafair
import os
from ctypes import cdll, c_void_p, c_int

LIBNAME = "libmu-toksia.so"
LIBPATH = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "build", "dynamic", "lib", LIBNAME)

semantics_to_code = {"AD": 0, "CO": 1, "PR": 2, "ST": 3, "SST": 4, "STG": 5, "ID": 6}

class mu_toksia(ipafair.AFSolver):

    def __init__(self, sigma: str, af_file: str = None):
        if not os.path.exists(LIBPATH):
            raise IOError("Shared library not found. Please run 'make' to build.")

        self.lib = cdll.LoadLibrary(LIBPATH)
        self.lib.ipafair_init.restype = c_void_p
        self.lib.ipafair_init.argtypes = []
        self.lib.ipafair_release.argtypes = [c_void_p]
        self.lib.ipafair_set_semantics.argtypes = [c_void_p, c_int]
        self.lib.ipafair_add_argument.argtypes = [c_void_p, c_int]
        self.lib.ipafair_del_argument.argtypes = [c_void_p, c_int]
        self.lib.ipafair_add_attack.argtypes = [c_void_p, c_int, c_int]
        self.lib.ipafair_del_attack.argtypes = [c_void_p, c_int, c_int]
        self.lib.ipafair_assume.argtypes = [c_void_p, c_int]
        self.lib.ipafair_solve_cred.restype = c_int
        self.lib.ipafair_solve_cred.argtypes = [c_void_p]
        self.lib.ipafair_solve_skept.restype = c_int
        self.lib.ipafair_solve_skept.argtypes = [c_void_p]
        self.lib.ipafair_val.restype = c_int
        self.lib.ipafair_val.argtypes = [c_void_p, c_int]

        self.solver = self.lib.ipafair_init()
        self.lib.ipafair_set_semantics(self.solver, semantics_to_code[sigma])
        self.status = 0
        self.args = set()
        self.last_call = None

        if af_file is not None:
            contents = [line.strip() for line in open(af_file).read().split("\n") if not line.startswith("#")]
            contents = [line for line in contents if len(line) > 0]
            p_line = contents[0]
            assert(p_line.startswith("p af "))
            n = int(p_line[5:])
            args = list(range(1, n+1))
            self.args = set(args)
            atts = [ list(map(int, line.split())) for line in contents[1:] ]
            assert(all(len(att) == 2 for att in atts))
            for a in args:
                self.add_argument(a)
            for s,t in atts:
                self.add_attack(s,t)

    def __del__(self):
        self.lib.ipafair_release(self.solver)

    def add_argument(self, arg: int):
        self.args.add(arg)
        self.lib.ipafair_add_argument(self.solver, arg)

    def del_argument(self, arg: int):
        self.args.remove(arg)
        self.lib.ipafair_del_argument(self.solver, arg)

    def add_attack(self, source: int, target: int):
        self.lib.ipafair_add_attack(self.solver, source, target)

    def del_attack(self, source: int, target: int):
        self.lib.ipafair_del_attack(self.solver, source, target)

    def solve_cred(self, assumps=[]) -> bool:
        for a in assumps:
            self.lib.ipafair_assume(self.solver, a)
        self.status = self.lib.ipafair_solve_cred(self.solver)
        self.last_call = True
        if self.status == 10:
            return True
        elif self.status == 20:
            return False
        return None

    def solve_skept(self, assumps=[]) -> bool:
        for a in assumps:
            self.lib.ipafair_assume(self.solver, a)
        self.status = self.lib.ipafair_solve_skept(self.solver)
        self.last_call = False
        if self.status == 10:
            return True
        elif self.status == 20:
            return False
        return None

    def extract_witness(self):
        if (self.status == 10 and self.last_call is True) or (self.status == 20 and self.last_call is False):
            extension = []
            for a in self.args:
                if self.lib.ipafair_val(self.solver, a) > 0:
                    extension.append(a)
            return extension
