import sys
from subprocess import Popen


def main():

    try:
        ncases = int(sys.argv[1], 10)
    except:
        ncases = 0

    print(f"[*] Performing tests for: {ncases} cases")

    # Use csmith to generate the cases

    return


if __name__ == "__main__":
    main()
