from pyspark.sql import SparkSession
from pyspark.sql import functions as F
import sys


def ind2d(i, j, tam):
    return i * (tam + 2) + j

def wall_time():
    from time import time
    return time()

def UmaVida(tabulIn, tabulOut, tam):
    def count_alive_neighbors(i, j):
        neighbors = [
            tabulIn[ind2d(i-1, j-1, tam)], tabulIn[ind2d(i-1, j, tam)], tabulIn[ind2d(i-1, j+1, tam)],
            tabulIn[ind2d(i, j-1, tam)],     tabulIn[ind2d(i, j+1, tam)],
            tabulIn[ind2d(i+1, j-1, tam)], tabulIn[ind2d(i+1, j, tam)], tabulIn[ind2d(i+1, j+1, tam)]
        ]
        return sum(neighbors)

    for i in range(1, tam+1):
        for j in range(1, tam+1):
            vizviv = count_alive_neighbors(i, j)
            if tabulIn[ind2d(i, j, tam)] and vizviv < 2:
                tabulOut[ind2d(i, j, tam)] = 0
            elif tabulIn[ind2d(i, j, tam)] and vizviv > 3:
                tabulOut[ind2d(i, j, tam)] = 0
            elif not tabulIn[ind2d(i, j, tam)] and vizviv == 3:
                tabulOut[ind2d(i, j, tam)] = 1
            else:
                tabulOut[ind2d(i, j, tam)] = tabulIn[ind2d(i, j, tam)]

def DumpTabul(tabul, tam, first, last, msg):
    def get_char(x):
        return 'X' if x else '.'

    print(f"{msg}; Dump posicoes [{first}:{last}, {first}:{last}] de tabuleiro {tam} x {tam}")
    for i in range(first, last + 1):
        print("=" * (last - first + 1))
        for j in range(first, last + 1):
            print(get_char(tabul[ind2d(i, j, tam)]), end="")
        print()
    print("=" * (last - first + 1))

def InitTabul(tam):
    tabulIn = [0] * ((tam + 2) * (tam + 2))
    tabulOut = [0] * ((tam + 2) * (tam + 2))

    tabulIn[ind2d(1, 2, tam)] = 1
    tabulIn[ind2d(2, 3, tam)] = 1
    tabulIn[ind2d(3, 1, tam)] = 1
    tabulIn[ind2d(3, 2, tam)] = 1
    tabulIn[ind2d(3, 3, tam)] = 1

    return tabulIn, tabulOut

def Correto(tabul, tam):
    cnt = sum(tabul)
    return cnt == 5 and tabul[ind2d(tam - 2, tam - 1, tam)] and tabul[ind2d(tam - 1, tam, tam)] and \
           tabul[ind2d(tam, tam - 2, tam)] and tabul[ind2d(tam, tam - 1, tam)] and tabul[ind2d(tam, tam, tam)]

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 jogo.py POWMIN POWMAX")
        sys.exit(1)

    try:
        POWMIN = int(sys.argv[1])
        POWMAX = int(sys.argv[2])
    except ValueError:
        print("POWMIN and POWMAX must be integer values")
        sys.exit(1)
    
    spark = SparkSession.builder.appName("ConwayGameOfLife").getOrCreate()

    result_str = ""
    for pow in range(POWMIN, POWMAX+1):
        tam = 1 << pow

        t0 = wall_time()
        tabulIn, tabulOut = InitTabul(tam)
        t1 = wall_time()

        for i in range(2 * (tam - 3)):
            UmaVida(tabulIn, tabulOut, tam)
            UmaVida(tabulOut, tabulIn, tam)

        t2 = wall_time()

        if Correto(tabulIn, tam):
            print("**RESULTADO CORRETO (Spark)**")
            result_str +="**RESULTADO CORRETO (Spark)**"
        else:
            print("**RESULTADO ERRADO (Spark)**")
            result_str +="**RESULTADO ERRADO (Spark)**"

        t3 = wall_time()
        print(f"tam={tam}; tempos: init={t1-t0:.7f}, comp={t2-t1:.7f}, fim={t3-t2:.7f}, tot={t3-t0:.7f}")
        result_str+= f"tam={tam}; tempos: init={t1-t0:.7f}, comp={t2-t1:.7f}, fim={t3-t2:.7f}, tot={t3-t0:.7f}"
    spark.stop()
    return result_str


if __name__ == "__main__":
    result = main()

    with open("outputspark.txt", "w") as file:
        file.write(result)
