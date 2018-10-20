

class A(object):
    def __getattr__(self, name):
        print "G"
        return object.__getattribute__(self, name)

    def __setattr__(self, name, value):
        print "s"
        pass

a = A()

a.x = 42
a.x

a.x = 43
a.x

