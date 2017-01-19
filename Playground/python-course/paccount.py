
class Account(object):

    __slots__ = [ 'name', 'balance', 'min_balance', 'transactions', 'index' ]

    def __init__(self, name):
        self.name = name
        self.balance = 0
        self.min_balance = 0
        self.transactions = []
        self.index = 0

    def __iter__(self):
        return self

    def next(self):
        if self.index < len(self.transactions):
            self.index += 1
            return self.transactions[self.index - 1]
        else:
            raise StopIteration

    def get_balance(self):
        return self.balance

    def deposit(self, value):
        self.balance += value
        self.transactions.append(value)
        self.check_balance()

    def withdraw(self, value):
        if value > self.balance - self.min_balance:
            raise Exception("insufficient balance")
        self.balance -= value
        self.transactions.append(-value)
        self.check_balance()

    def check_balance(self):
        assert sum(self.transactions) == self.balance


class PAccount(Account):
    __slots__ = []
    def __init__(self, name, amount):
        Account.__init__(self, name)
        self.min_balance = -500
        self.deposit(amount)

account = PAccount('sue', 100)

assert account.get_balance() == 100

account.withdraw(100)
assert account.get_balance() == 0

print account.balance


try:
    account.withdraw(500)
    assert False
except Exception as e:
    assert account.get_balance() == -500


print "Transactions:"
for trans in account:
    print "    ", trans

print "Everything is OK."
