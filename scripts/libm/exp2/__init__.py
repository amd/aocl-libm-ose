from scripts.libm import exp

class Exp2(exp.Exp):
    def __init__(self, table_size):
	super().__init__(table_size)
