import gdb.printing

class WordPrinter(object):
    def __init__(self, val):
        self.val = val

    def to_string(self):
        hi = self.val['m_hi'].referenced_value()
        lo = self.val['m_lo'].referenced_value()
        #return ''.join('%02x' % a for a in [hi, lo])
        return '0x%02x%02x' % (hi, lo)

def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("dmgb")
    pp.add_printer('word', '^word$', WordPrinter)
    return pp

gdb.printing.register_pretty_printer(gdb.current_objfile(), build_pretty_printer())
