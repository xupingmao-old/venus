def str_ljust(self, num):
    if len(self) >= num: return self
    rest = num - len(self)
    return self + rest * ' '

add_type_method('str', 'ljust', str_ljust)

def str_rjust(self, num):
    if len(self) >= num: return self
    rest = num - len(self)
    return rest * ' ' + self
add_type_method('str', 'rjust', str_rjust)