/* D code example 
 * - D Programming Language -
 *    The improved C 
 * Designed for educational purposes
 * Developed by: Thealg0
 *
*/


// Exemplo de comentário de linha

/*
   Exemplo de comentário de bloco
*/

// Declaração de variáveis

var input
var base
var threshold
var counter
var limit
var sum
var difference
var greeting
var status
var numbers[3]

// Entrada de dados


print ("Digite um número inteiro: ")
scan (input)

print ("Digite um numero decimal: ")
scan (base)

print ("Digite uma saudação: ")
scans (greeting)

print ("Digite um status: ")
scans (status)

// Atribuições e operações

base = 10
threshold = 25
difference = base - input

// Estruturas condicionais

if (difference < 0) {
    print (-difference)
}
else {
    print (difference)
}



numbers[0] = input
numbers[1] = base + 5
numbers[2] = numbers[1] / 2

counter = 0
limit = 3
sum = 0

// Laço de repetição

while (counter < limit) {
    sum = sum + counter * numbers[2]
    counter = counter + 1
}

print (numbers[0])
print (numbers[1])
print (numbers[2])
print (sum)

if (numbers[1] == numbers[2]) {
    print (0)
}
else {
    print (numbers[1] - numbers[2])
}

printt (greeting)
printt (status)


// Resolução do Cálculo da média ponderada

var nota1
var nota2
var peso1
var peso2
var media_ponderada
var soma_pesos
printt ("Digite a primeira nota: ")

scan (nota1)

printt ("Digite o peso da primeira nota: ")
scan (peso1)
printt ("Digite a segunda nota: ")
scan (nota2)
printt ("Digite o peso da segunda nota: ")
scan (peso2)


// Cálculo da média ponderada
soma_pesos = peso1 + peso2
media_ponderada = (nota1 * peso1 + nota2 * peso2) / soma_pesos
print (media_ponderada)