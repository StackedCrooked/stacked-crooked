# dummy placeholder
class Trace(object):
    def __init__(self, name):
        self.name = name

    def __get__(self, obj, objtype):
        print("GET", self.name, str(obj.__dict__[self.name]))
        return obj.__dict__[self.name]

    def __set__(self, obj, value):
        print("SET", self.name, value)
        obj.__dict__[self.name] = value


class Point(object):
    def __init__(self, x, y):
        self.__dict__['x'] = x
        self.__dict__['y'] = y

    def moveBy(self, dx, dy):
        self.x += dx
        self.y += dy

    def setX(self, value):
        print("setX()")
        self.__dict__['x'] = value

    def getX(self):
        print("getX()")
        return self.__dict__['x']

    x = property(getX, setX)
    y = Trace("y")


p = Point(3, 4)
p.moveBy(10, 20)
p.moveBy(10, 20)
p.moveBy(10, 20)
