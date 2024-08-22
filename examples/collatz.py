def collatz(num):
    x = 0
    while num != 1:
        if num % 2 == 0:
            num = num // 2
        else:
            num = 3 * num + 1
        x += 1
    print(x)


while True:
    try:
        x = input()
        collatz(int(x))
    except:
        break
