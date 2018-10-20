import sys

class Account(object):

    __slots__ = [ 'name', '_balance', 'min_balance', 'transactions', 'index' ]

    def __init__(self, name):
        self.name = name
        self._balance = 0
        self.min_balance = 0
        self.transactions = []
        self.index = 0

    def __iter__(self):
        return self

    def __getitem__(self, i):
        return self.transactions[i]

    def next(self):
        if self.index < len(self.transactions):
            self.index += 1
            return self.transactions[self.index - 1]
        else:
            raise StopIteration

    @property
    def balance(self):
        return self._balance

    def deposit(self, value):
        self._balance += value
        self.transactions.append(value)
        self.check_balance()

    def withdraw(self, value):
        if value > self._balance - self.min_balance:
            raise Exception("insufficient balance")
        self._balance -= value
        self.transactions.append(-value)
        self.check_balance()

    def check_balance(self):
        assert sum(self.transactions) == self._balance


class PAccount(Account):
    __slots__ = []
    def __init__(self, name, amount):
        Account.__init__(self, name)
        self.min_balance = -500
        self.deposit(amount)


account = PAccount('sue', 100)
assert account.balance == 100


account.withdraw(100)
assert account.balance == 0


try:
    account.withdraw(1000)
    assert False # an exception should have been thrown
except Exception as e:
    print("OK. I can't withdraw 1000")
    assert account.balance == 0

print("Transactions:")

print("account[:] => ", account[:])

print("Everything is OK.")


class A(object):
    pass

a = A();
a.x = 1
a.x = 1
