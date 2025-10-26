import os
import sys
import subprocess


GREEN = "\033[92m"
WHITE = "\033[97m"
RED = "\033[91m"


def shell(s: str, ret_code_exp=0):
    ret_code = os.system(s)
    if ret_code != ret_code_exp * 256:
        print(RED + f"FAIL: shell command **{s}**")
        print(f"ret_code={ret_code // 256}, expected={ret_code_exp}" + WHITE)
        exit()


def test_common():
    for ERROR in (
        "EACCES",
        "EFAULT",
        "EINVAL",
    ):
        shell(f"rm -f TEST.in TEST.out TEST.out.1")
        shell(f"echo 123 | tee TEST.in > TEST.out.1")
        shell(f"strace 2> /dev/null -P 'TEST.in' -e fault=access:error={ERROR}:when=1 ./move TEST.in TEST.out > /dev/null", 2)
        shell(f"strace 2> /dev/null -P 'TEST.in' -e fault=openat:error={ERROR}:when=1 ./move TEST.in TEST.out > /dev/null", 3)
        shell(f"strace 2> /dev/null -P 'TEST.in' -e fault=lseek:error={ERROR}:when=1 ./move TEST.in TEST.out > /dev/null", 4)
        shell(f"strace 2> /dev/null -P 'TEST.in' -e fault=lseek:error={ERROR}:when=2 ./move TEST.in TEST.out > /dev/null", 5)
        shell(f"strace 2> /dev/null -P 'TEST.in' -e fault=read:error={ERROR}:when=1 ./move TEST.in TEST.out > /dev/null", 6)
        shell(f"strace 2> /dev/null -P 'TEST.in' -e fault=close:error={ERROR}:when=1 ./move TEST.in TEST.out > /dev/null", 8)
        shell(f"test -e TEST.out", 1)
        shell(f"strace 2> /dev/null -P 'TEST.out' -e fault=openat:error={ERROR}:when=1 ./move TEST.in TEST.out > /dev/null", 9)
        # почему-то write не может быть отслежен по файлу (?) как будто strace видит другое имя файла
        # ОС: ubuntu 25.4
        shell(f"strace 2> /dev/null               -e fault=write:error={ERROR}:when=1 ./move TEST.in TEST.out > /dev/null", 10)
        shell(f"test -e TEST.out", 0)
        shell(f"rm TEST.out", 0)
        shell(f"strace 2> /dev/null -P 'TEST.in' -e fault=unlink:error={ERROR} ./move TEST.in TEST.out > /dev/null", 12)
        shell(f"test -e TEST.out", 0)


def test_ok():
    shell("echo 123 | tee OK.in > OK.out.1")
    shell("./move OK.in OK.out > /dev/null")
    shell("cmp OK.out OK.out.1")
    shell("test -e OK.in", 1)

    shell("./move OK2.in OK2.out > /dev/null", 2)


def test_protect():
    shell("echo 123 | tee NORM.in > NORM.out.1")
    shell("LD_PRELOAD=`pwd`/evil_unlink.so ./move NORM.in NORM.out > /dev/null")
    shell("test -e NORM.in", 1)
    shell("echo 123 | tee PROTECT.in > PROTECT.out.1")
    shell("LD_PRELOAD=`pwd`/evil_unlink.so ./move PROTECT.in PROTECT.out > /dev/null")
    shell("cmp PROTECT.in PROTECT.out")
    shell("cmp PROTECT.out PROTECT.out.1")


if __name__ == "__main__":
    globals()[sys.argv[1]]()
    print(GREEN + f"SUCCESS: {sys.argv[1]}" + WHITE)
