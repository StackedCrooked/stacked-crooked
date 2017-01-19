

class Account(object):
    def __init__(self, name):
        self.name = name
        self.balance = 0
        self.transactions = []

    def get_balance(self):
        return self.balance

    def deposit(self, value):
        self.balance += value
        self.transactions.append(value)
        self.check_balance()

    def withdraw(self, value):
        self.balance -= value
        self.transactions.append(-value)
        self.check_balance()

    def check_balance(self):
        assert sum(self.transactions) == self.balance

account = Account("test-account")

assert account.name == "test-account"
assert account.get_balance() == 0

account.deposit(100)
assert account.get_balance() == 100

account.withdraw(100)
assert account.get_balance() == 0

account.withdraw(10000)
assert account.get_balance() == -10000

print(account.transactions)

print("Everything is OK.")

