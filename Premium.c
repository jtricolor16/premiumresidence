#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <ctype.h>

#define X 4																	//N° de blocos
#define Y 10																//N° de andares
#define Z 6																	//N° de apartamentos por andar
#define G 400																//N° de vagas de garagem
#define V 20000																//Preço da vaga de garagem
#define P 0.035																//Percentual de aumento do preço do apartamento por acréscimo de andar
#define Q1 225000															//Preço do apartamento de 1 quarto
#define Q2 280000															//Preço do apartamento de 2 quartos
#define Q3 375000															//Preço do apartamento de 3 quartos
#define C 0.004																//Percentual da comissão do vendedor
#define F 20																//Quantidade de vendedores
double const tempo=172800;													//Dois dias em segundos

//Estrututas de dados

typedef struct{																//Definição do tipo Data
	int dia;
	int mes;
	int ano;
}TData;

typedef struct{																//Definição do tipo Cliente
	char nome[50];
	float cpf, telefone;
} TCliente;

typedef struct{																//Definição do tipo Vendedor
	int codigo;
	char nome[50];
	TData data;
} TVendedor;

typedef struct{																//Definição do tipo Apartamento
	int bloco, und, nquartos, status, ngar, tcompra;
	TCliente cliente;
	TVendedor vendedor;
	time_t tinicio;
	float valorb, valorl, comissao;	
} TApe;


//Protótipos de arquivo

int origem(TApe ape[X][Y][Z], char nomedoarquivo[]);						//Criar um arquivo binario com os dados originais do empreendimento
int lebin(TApe ape[X][Y][Z], char nomedoarquivo[]);							//Ler um arquivo binário
int atual(TApe ape[X][Y][Z], char nomedoarquivo[]);							//Criar o arquivo após concluir o cadastro de dados
int atualg(int vgar[], char nomedoarquivo[]);								//Criar o arquivo com os dados de garagem após concluir o cadastro de dados
int lebing(int vgar[], char nomedoarquivo[]);								//Ler um arquivo binário de garagem
void limpabuffer();

//Protótipos de movimentação

int menu();                                    								//Função para o Menu Inicial 
void cliente(TApe ape[X][Y][Z], int bl, int und);							//Função para cadastrar os dados dos clientes
void vendedor(TApe ape[X][Y][Z], int bl, int und);  						//Função para cadastrar os dados dos vendedores
int pgaragem(TApe ape[X][Y][Z], int bl, int und);							//Função para decidir se o cliente comprará ou não vagas de garagem
int ngaragem(TApe ape[X][Y][Z], int bl, int und);							//Função para decidir número de vagas a serem compradas
void zeravet(int vgar[]);													//Função para zerar o vetor garagem
void garagem(TApe ape[X][Y][Z], int bl, int und, int vgar[], int ngar);		//Função para inserir vaga(s) escolhida(s)
void tcompra(TApe ape[X][Y][Z], int bl, int und);							//Funcao para registrar o tipo da compra e calcular o valor líquido	
void comissao(TApe ape[X][Y][Z], int bl, int und);							//Funcao para calcular comissao do vendedor
void rreserva(TApe ape[X][Y][Z]);											//Função para liberar um apartamento reservado
void zerafloat(float tcomissao[]);											//Função para zerar o vetor tcomissão

//Protótipos de relatório

int menu_relatorio(TApe ape[X][Y][Z]);										//Função para o Menu Relatório
void relatorio1(TApe ape[X][Y][Z]);											//Disponibilidade de apartamentos do condomínio
void relatorio2(TApe ape[X][Y][Z]);											//Disponibilidade de apartamentos por bloco do condomínio
void relatorio3(TApe ape[X][Y][Z]);											//Disponibilidade de apartamentos por número de quartos
void relatorio4(int vgar[G]);												//Disponibilidade de vagas de garagem
void relatorio5(TApe ape[X][Y][Z], int vgar[G]);							//Vagas adquiridas
void relatorio6(TApe ape[X][Y][Z]);											//Consulta de apartamentos por status (total ou por bloco)
void relatorio7(TApe ape[X][Y][Z]);											//Comissao dos vendedores por mês
void relatorio8(TApe ape[X][Y][Z]);											//Reserva dos clientes por vendedor

//Funções de arquivo
		
int origem(TApe ape[X][Y][Z], char nomedoarquivo[]){
	int i, j, k, l;
	FILE *arq1;		
	arq1 = fopen(nomedoarquivo,"wb");										//Criação do arquivo dos apartamentos
		
	if(!arq1){
		printf("Erro!");
		return 0;
	} else{
		for(i=0; i<X; i++){
			for(j=0; j<Y; j++){
				for(k=0; k<Z; k++){
					ape[i][j][k].bloco = i+1;								//Gerador do nº do bloco
					ape[i][j][k].und = k+1 + ((j+1)*100);					//Gerador do nº do apartamento
					ape[i][j][k].status = 0;								//Zerar o status (0 Disponível, 1 Compra, 2 Reserva e 3 Construtora)					
					ape[i][j][k].cliente.cpf = 0;							//Zerar os valores do CPF cliente
					ape[i][j][k].vendedor.codigo = 0;						//Zerar os valores do código Vendedor
					ape[i][j][k].vendedor.data.dia = 0;						//Zerar o valor do dia da venda/reserva
					ape[i][j][k].vendedor.data.mes = 0;						//Zerar o valor do mês da venda/reserva
					ape[i][j][k].vendedor.data.ano = 0;						//Zerar o valor do ano da venda/reserva
					if(k == 1 || k == 4){
						ape[i][j][k].nquartos = 3;							//Gerador do nº de quartos do apartamento (3 quartos para colunas 2 e 5)
						ape[i][j][k].valorb = Q3 * pow(1+P,j);				//Gerador do valor bruto do apartamento (aumenta o valor conforme acréscimo do andar)
					} else if(k == 0 || k == 3){
						ape[i][j][k].nquartos = 2;							//Gerador do nº de quartos do apartamento (2 quartos para colunas 1 e 4)
						ape[i][j][k].valorb = Q2 * pow(1+P,j);				//Gerador do valor bruto do apartamento (aumenta o valor conforme acréscimo do andar)
					} else{
						ape[i][j][k].nquartos = 1;							//Gerador do nº de quartos do apartamento (1 quartos para colunas 3 e 6)
						ape[i][j][k].valorb = Q1 * pow(1+P,j);				//Gerador do valor bruto do apartamento (aumenta o valor conforme acréscimo do andar)
					}
					ape[i][j][k].ngar = 0;									//Zerar o valor do número de garagem					
				}				
			}					
		}
		fwrite(ape, sizeof(TApe),X*Y*Z, arq1);										
	}			
	fclose(arq1);
	return 1;
}
	
int lebin(TApe ape[X][Y][Z], char nomedoarquivo[]){
	FILE *arq1;

	arq1 = fopen(nomedoarquivo, "rb");
	
	if(!arq1){
		printf("Erro!");
		return 0;
	} else {
		fread(ape,sizeof(TApe), X*Y*Z, arq1);
	}
	fclose(arq1);
	return 1;
}

int atual(TApe ape[X][Y][Z], char nomedoarquivo[]){
	
	FILE *arq1;		
	arq1 = fopen(nomedoarquivo,"wb");
		
	if(!arq1){
		printf("Erro!");
		return 0;
	} else{
		fwrite(ape, sizeof(TApe),X*Y*Z, arq1);										
	}			
	fclose(arq1);
	return 1;
}

int lebing(int vgar[], char nomedoarquivo[]){
	FILE *arq1;

	arq1 = fopen(nomedoarquivo, "rb");
	
	if(!arq1){
		printf("Erro!");
		return 0;
	} else {
		fread(vgar,sizeof(int), G, arq1);
	}
	fclose(arq1);
	return 1;
}

int atualg(int vgar[], char nomedoarquivo[]){
	FILE *arq1;		
	arq1 = fopen(nomedoarquivo,"wb");
		
	if(!arq1){
		printf("Erro!");
		return 0;
	} else{
		fwrite(vgar, sizeof(int), G, arq1);										
	}			
	fclose(arq1);
	return 1;
}



//Funções de movimentação

int menu(){                                                    				
    int c;
    printf("\n");
    printf("-------------------------------------\n");
    printf("AL2 PREMIUM RESIDENCE\n");
    printf("-------------------------------------\n\n");
    printf("GERENCIAMENTO DE APARTAMENTOS\n\n\n\n");
    printf("-> 1 - VENDAS E RESERVAS\n\n\n");
    printf("-> 2 - RELATORIOS\n\n\n");
    printf("-> 9 - SAIR\n\n");
    scanf("%d",&c);
    limpabuffer();
    switch(c){
		case 1:
		    return c;
            break;
		case 2:
           return c;
           break;
        case 9:
            return c;
            break;
        default:
		    {
            system("cls");
           	printf("*POR FAVOR DIGITE UMA DAS OPCOES!*");
           	c=menu();
           	break;
            }
    }
}

void cliente(TApe ape[X][Y][Z], int bl, int und){
	printf("DADOS DO CLIENTE\n");
	printf("-> NOME: ");
    gets(ape[bl-1][und/100-1][und%100-1].cliente.nome);						//Guarda o nome do cliente no apartamento escolhido
    printf("-> CPF: ");
    scanf("%f", &ape[bl-1][und/100-1][und%100-1].cliente.cpf);						//Guarda o CPF do cliente no apartamento escolhido
    limpabuffer();
	printf("-> TELEFONE: ");
    scanf("%f", &ape[bl-1][und/100-1][und%100-1].cliente.telefone);					//Guarda o telefone do cliente no apartamento escolhido
    limpabuffer();
	printf("\n");
}

void vendedor(TApe ape[X][Y][Z], int bl, int und){
	printf("DADOS DO VENDEDOR\n");
    printf("-> CODIGO: ");
   	scanf("%d", &ape[bl-1][und/100-1][und%100-1].vendedor.codigo);    				//Guarda o código do vendedor no apartamento escolhido
   	limpabuffer();
	printf("-> NOME: ");
    gets(ape[bl-1][und/100-1][und%100-1].vendedor.nome);						//Guarda o nome do vendedor no apartamento escolhido
	printf("-> DATA:\n");
   	printf("      DD: ");
   	scanf("%d", &ape[bl-1][und/100-1][und%100-1].vendedor.data.dia);
   	limpabuffer();
	printf("      MM: ");
   	scanf("%d", &ape[bl-1][und/100-1][und%100-1].vendedor.data.mes);
   	limpabuffer();
	printf("      AAAA: ");
   	scanf("%d", &ape[bl-1][und/100-1][und%100-1].vendedor.data.ano);
 	limpabuffer();
	printf("\n");
}

int pgaragem(TApe ape[X][Y][Z], int bl, int und){
	int c;	
	printf("DESEJA ADQUIRIR VAGAS DE GARAGEM.\n     Preco unitario: %d.00.\n1 - SIM\n0 - NAO\n",V);
	scanf("%d", &c);
	limpabuffer();
	switch(c){																		/*	switch para a aquisição de vagas de garagem	*/
		case 1:
			return c;
			break;
		case 0:	
			system ("cls");
			printf ("\nVOCE NAO ADQUIRIU VAGAS DE GARAGEM\n");
			system("PAUSE");
			return c;
			break;
		default:
			{
			system("cls");
           	printf("*POR FAVOR DIGITE UMA DAS OPCOES!*");
           	c=pgaragem(ape, bl, und);
           	system("PAUSE");
           	break;
			}		
	}
}

int ngaragem(TApe ape[X][Y][Z], int bl, int und){
	int qgar;
	if(ape[bl-1][und/100-1][und%100-1].nquartos==3){
		printf("\nVOCE PODE ADQUIRIR ATE DUAS VAGAS DE GARAGEM.\n");
		printf("     Informe a quantidade de vagas:\n");
		scanf("%d", &qgar);
		limpabuffer();
		switch(qgar){
			case 1:
				ape[bl-1][und/100-1][und%100-1].ngar=1;
				return qgar;
				break;
			case 2:
				ape[bl-1][und/100-1][und%100-1].ngar=2;
				return qgar;
				break;
			default:
				{						
				system("cls");
				printf("\nQUANTIDADE DE VAGAS INAPROPRIADA\n");
				system("PAUSE");
				return qgar=ngaragem(ape, bl, und);
				break;
				}
		}
	}else{
		qgar=1;
		ape[bl-1][und/100-1][und%100-1].ngar=1;
		return qgar;	
	}
}

void zeravet(int vgar[]){
	int i;
	for(i=0; i<G; i++){
		vgar[i]=0;
	}
}							
					
void garagem(TApe ape[X][Y][Z], int bl, int und, int vgar[], int ngar){
	int qgar, i, cont=0;
	
	if(ngar==2){
		while(cont<2){
			printf("\nDIGITE O NUMERO DA VAGA QUE DESEJA COMPRAR (1-%d)\n",G);
			scanf ("%d", &qgar);
			limpabuffer();
			if(qgar<1 || qgar>G){
				system("cls");
				printf("\nVAGA INEXISTENTE\n");
				system("PAUSE");
				garagem(ape, bl, und, vgar, ngar);
			} else{
				if(vgar[qgar-1]!=0 && cont==0){
					system("cls");
					printf("\nVAGA G%d OCUPADA\n", qgar);
					system("PAUSE");
					garagem(ape, bl, und, vgar, ngar);
					break;
				} else if(vgar[qgar-1]!=0 && cont==1){
					system("cls");
					printf("\nVAGA G%d OCUPADA\n", qgar);
					printf("\nA PRIMEIRA VAGA FOI ADQUIRIDA. COMPRE A SEGUNDA.\n");
					system("PAUSE");
					ngar=1;
					garagem(ape, bl, und, vgar, ngar);
					break;
				} else{
					vgar[qgar-1]=und+bl;
					cont++;
				}
			}
		}
	} else{
		printf("\nDIGITE O NUMERO DA VAGA QUE DESEJA COMPRAR (1-%d)\n",G);
		scanf("%d", &qgar);
		limpabuffer();
		if(qgar<1 || qgar>G){
			system("cls");
			printf("\nVAGA INEXISTENTE\n");
			system("PAUSE");
			garagem(ape, bl, und, vgar, ngar);
		} else{
			if(vgar[qgar-1]!=0){
				system("cls");
				printf("\nVAGA G%d OCUPADA\n", qgar);
				system("PAUSE");
				garagem(ape, bl, und, vgar, ngar);
			} else{
				vgar[qgar-1]=und+bl;
			}
		}
	}
}
														
void tcompra(TApe ape[X][Y][Z], int bl, int und){						
	int c;
	float d;
	char p = '%';	
	printf("\nESCOLHA A FORMA DE PAGAMENTO.\n     1 - A VISTA\n     2 - FINANCIAMENTO\n");
	scanf("%d", &c);
	limpabuffer();
	switch(c){														
		case 1:
			printf("\nInforme o percentual de desconto (ate 15%c).\n",p);
			printf("     Digite 0 se nao houver desconto.\n");
			scanf("%f", &d);
			limpabuffer();		
			if(d>=0 && d<=15){
				ape[bl-1][und/100-1][und%100-1].valorl=(ape[bl-1][und/100-1][und%100-1].valorb-(ape[bl-1][und/100-1][und%100-1].valorb*d/100))+(ape[bl-1][und/100-1][und%100-1].ngar*(V-(V*d/100)));
				printf("\nPreco liquido da unidade %d do bloco %d: %.2f\n",und,bl,ape[bl-1][und/100-1][und%100-1].valorl);
			} else {
				system("cls");
           		printf("*PERCENTUAL DE DESCONTO INVALIDO!\n*");
           		system("PAUSE");
				tcompra(ape, bl, und);           		
           	}
			break;		
		case 2:
			printf("\nInforme o percentual de desconto (ate 5%c).\n",p);
			printf("     Digite 0 se nao houver desconto.\n");
			scanf("%f", &d);
			limpabuffer();		
			if(d>=0 && d<=5){
				ape[bl-1][und/100-1][und%100-1].valorl=(ape[bl-1][und/100-1][und%100-1].valorb-(ape[bl-1][und/100-1][und%100-1].valorb*d/100))+(ape[bl-1][und/100-1][und%100-1].ngar*(V-(V*d/100)));
				printf("\nPreco liquido da unidade %d do bloco %d: %.2f\n",und,bl,ape[bl-1][und/100-1][und%100-1].valorl);
			} else {
				system("cls");
           		printf("*PERCENTUAL DE DESCONTO INVALIDO!\n*");
           		system("PAUSE");
				tcompra(ape, bl, und);           		
           	}
			break;		
		default:
			{
			system("cls");
           	printf("*POR FAVOR DIGITE UMA DAS OPCOES!*\n");
           	system("PAUSE");
           	tcompra(ape, bl, und);
			break;
			}				
	}
}

void comissao(TApe ape[X][Y][Z], int bl, int und){							
	char p = '%';
		ape[bl-1][und/100-1][und%100-1].comissao=ape[bl-1][und/100-1][und%100-1].valorl*C;
		printf("\nValor da comissao (0.4%c): %.2f\n",p,ape[bl-1][und/100-1][und%100-1].comissao);
}

void rreserva(TApe ape[X][Y][Z]){
	int i,j,k;
	time_t tfim;
	time(&tfim);
	for(i=0; i<X; i++){
		 for(j=0;j<Y;j++){
		 	for(k=0;k<Z;k++){
		 		if(ape[i][j][k].status==2){
		 			if(difftime(tfim,ape[i][j][k].tinicio)>tempo) {
						ape[i][j][k].status=0;
					}
				}
			 }
		 }
	}
}

void zerafloat(float tcomissao[]){
	int i;
	for(i=0; i<F; i++){
		tcomissao[i]=0;
	}
}

//Funções dos relatórios

int menu_relatorio(TApe ape[X][Y][Z]){
	int r;
	printf("\n");
    printf("-------------------------------------\n");
    printf("AL2 PREMIUM RESIDENCE\n");
    printf("-------------------------------------\n\n");
    printf("RELATORIOS\n\n");
    printf("-> 1 - DISPONIBILIDADE DE APARTAMENTOS TOTAL\n\n");
    printf("-> 2 - DISPONIBILIDADE DE APARTAMENTOS POR BLOCO\n\n");
    printf("-> 3 - DISPONIBILIDADE DE APARTAMENTOS POR No DE QUARTOS\n\n");
    printf("-> 4 - VAGAS DE GARAGEM DISPONIVEIS\n\n");
    printf("-> 5 - VAGAS DE GARAGEM OCUPADAS\n\n");
    printf("-> 6 - CONSULTA STATUS (VENDIDO, RESERVADO, DISPONIVEL OU CONSTRUTORA)\n\n");
    printf("-> 7 - COMISSAO DE VENDEDORES POR MES\n\n");
    printf("-> 8 - LISTA DE RESERVAS POR VENDEDOR\n\n");
    printf("-> 9 - RETORNAR AO MENU INICIAL\n\n");
    scanf("%d",&r);
	limpabuffer();
	rreserva(ape);
	switch(r){
		case 1:
		    return r;
            break;
		case 2:
           return r;
           break;
        case 3:
		    return r;
            break;
		case 4:
           return r;
           break;
		case 5:
		    return r;
            break;
		case 6:
           return r;
           break;
		case 7:
		    return r;
            break;        
        case 8:
            return r;
            break;
        case 9:
			return r;
			break;    
        default:
		    {
            system("cls");
           	printf("*POR FAVOR DIGITE UMA DAS OPCOES!*\n");
           	system("PAUSE");
           	r=menu_relatorio(ape);
           	break;
            }
    }
}

void relatorio1(TApe ape[X][Y][Z]){
	int i, j, k, r;
	FILE *arq1;
	char linha[80];
	
	printf("DISPONIBILIDADE DE APARTAMENTOS - AL2 PREMIUM RESIDENCE\n");
	printf("\nBl Und\tBl Und\tBl Und\tBl Und\tBl Und\tBl Und\n");
	
	for(i=0; i<X; i++){
		for(j=0; j<Y;j++){
			for(k=0;k<Z;k++){
				if(ape[i][j][k].status==0){
					printf("%d ", ape[i][j][k].bloco);
					printf("%d\t", ape[i][j][k].und);
				}
			}
			printf("\n");
		}
		printf("\n");
	}
	printf("\n1 - SALVAR EM ARQUIVO\n0 - RETORNAR AO MENU INICIAL\n");
	scanf("%d", &r);
	limpabuffer();
	switch(r){
		case 0:
			system("cls");
           	break;
		case 1:
			arq1=fopen("condominio.txt", "w");
			if(!arq1){
				printf("Erro na geracao do arquivo!");
			} else{
				fprintf(arq1, "DISPONIBILIDADE DE APARTAMENTOS - AL2 PREMIUM RESIDENCE\n", linha);
				fprintf(arq1,"\nBl Und\tBl Und\tBl Und\tBl Und\tBl Und\tBl Und\n", linha);
				for(i=0; i<X; i++){
					for(j=0; j<Y;j++){
						for(k=0;k<Z;k++){
							if(ape[i][j][k].status==0){
								fprintf(arq1,"%d ", ape[i][j][k].bloco);
								fprintf(arq1,"%d\t", ape[i][j][k].und);
							}
						}
					fprintf(arq1,"\n", linha);
					}
				fprintf(arq1,"\n", linha);
				}	
			}
			fclose(arq1);
			printf("\nARQUIVO GERADO COM SUCESSO!\n");
			system("PAUSE");
			break;
		default:
			system("cls");
           	printf("*POR FAVOR DIGITE UMA DAS OPCOES!*\n");
           	system("PAUSE");
           	break;		
	}
	
}

void relatorio2(TApe ape[X][Y][Z]){
	int j, k, r, bl;
	char nomedoarquivo[20];
	FILE *arq1;
	char linha[80];
	
	printf("DIGITE O BLOCO DESEJADO\n      1 - Array\n      2 - Matrix\n      3 - Struct\n      4 - File\n");
	scanf("%d", &bl);
	limpabuffer();
	if(bl>0 && bl <5){
    	printf("\nDISPONIBILIDADE DE APARTAMENTOS - BLOCO %d - AL2 PREMIUM RESIDENCE\n", bl);
		printf("\nBl Und\tBl Und\tBl Und\tBl Und\tBl Und\tBl Und\n");
		for(j=0; j<Y;j++){
			for(k=0;k<Z;k++){
				if(ape[bl-1][j][k].status==0){
					printf("%d ", ape[bl-1][j][k].bloco);
					printf("%d\t", ape[bl-1][j][k].und);
				}
			}
			printf("\n");
		}
		printf("\n");
		
		if(bl==1){
			strcpy(nomedoarquivo, "DispArray.txt");
		} else if(bl==2){
			strcpy(nomedoarquivo, "DispMatrix.txt");
		} else if(bl==3){
			strcpy(nomedoarquivo, "DispStruct.txt");
		} else{
			strcpy(nomedoarquivo, "DispFile.txt");
		}
	
		printf("\n1 - SALVAR EM ARQUIVO\n0 - RETORNAR AO MENU INICIAL\n");
		scanf("%d", &r);
		limpabuffer();
		switch(r){
			case 0:
				system("cls");
        	   	break;
			case 1:
				arq1=fopen(nomedoarquivo, "w");
				if(!arq1){
					printf("Erro na geracao do arquivo!");
				} else{
					fprintf(arq1, "DISPONIBILIDADE DE APARTAMENTOS POR BLOCO - AL2 PREMIUM RESIDENCE\n", linha);
					fprintf(arq1,"\nBl Und\tBl Und\tBl Und\tBl Und\tBl Und\tBl Und\n", linha);
					for(j=0; j<Y;j++){
						for(k=0;k<Z;k++){
							if(ape[bl-1][j][k].status==0){
								fprintf(arq1,"%d ", ape[bl-1][j][k].bloco);
								fprintf(arq1,"%d\t", ape[bl-1][j][k].und);
							}
						}
					fprintf(arq1,"\n", linha);
					}	
				}
				fclose(arq1);
				printf("\nARQUIVO GERADO COM SUCESSO!\n");
				system("PAUSE");
				break;
			default:
				system("cls");
           		printf("*POR FAVOR DIGITE UMA DAS OPCOES!*\n");
           		system("PAUSE");
           		break;		
		}
	} else{
		system("cls");
		printf("OPCAO INVALIDA!\n");
		system("PAUSE");
	}
}

void relatorio3(TApe ape[X][Y][Z]){
	int i, j, k, r, nquar;
	char nomedoarquivo[20];
	FILE *arq1;
	char linha[80];
	
	printf("DIGITE O NUMERO DE QUARTOS (1, 2 OU 3)\n");
	scanf("%d", &nquar);
	limpabuffer();
	if(nquar>0 && nquar<4){
    	printf("\nDISPONIBILIDADE DE APARTAMENTOS - %d QUARTO(S)- AL2 PREMIUM RESIDENCE\n", nquar);
		printf("\nBl Und\tBl Und\n");
		
		for(i=0;i<X;i++){
			for(j=0; j<Y;j++){
				for(k=0;k<Z;k++){
						if(ape[i][j][k].status==0 && ape[i][j][k].nquartos==nquar){
							printf("%d ", ape[i][j][k].bloco);
							printf("%d\t", ape[i][j][k].und);
						}
				}
				printf("\n");
			}
			printf("\n");
		}
		
		if(nquar==1){
			strcpy(nomedoarquivo, "Disp1Quarto.txt");
		} else if(nquar==2){
			strcpy(nomedoarquivo, "Disp2Quartos.txt");
		} else{
			strcpy(nomedoarquivo, "Disp3Quartos.txt");
		}
		
		printf("\n1 - SALVAR EM ARQUIVO\n0 - RETORNAR AO MENU INICIAL\n");
		scanf("%d", &r);
		limpabuffer();
		switch(r){
			case 0:
				system("cls");
        	   	break;
			case 1:
				arq1=fopen(nomedoarquivo, "w");
				if(!arq1){
					printf("Erro na geracao do arquivo!");
				} else{
					fprintf(arq1, "DISPONIBILIDADE DE APARTAMENTOS POR No QUARTOS - AL2 PREMIUM RESIDENCE\n", linha);
					fprintf(arq1,"\nBl Und\tBl Und\n", linha);
					for(i=0;i<X;i++){
						for(j=0; j<Y;j++){
							for(k=0;k<Z;k++){
								if(ape[i][j][k].status==0 && ape[i][j][k].nquartos==nquar){
									fprintf(arq1, "%d ", ape[i][j][k].bloco);
									fprintf(arq1, "%d\t", ape[i][j][k].und);							
								}								
							}
							fprintf(arq1,"\n", linha);	
						}
						fprintf(arq1,"\n", linha);
					}
				}
				fclose(arq1);
				printf("\nARQUIVO GERADO COM SUCESSO!\n");
				system("PAUSE");
				break;
			default:
				system("cls");
           		printf("*POR FAVOR DIGITE UMA DAS OPCOES!*\n");
           		system("PAUSE");
           		break;		
		}
	} else{
		system("cls");
		printf("NUMERO DE QUARTOS INVALIDO!\n");
		system("PAUSE");
	}
}

void relatorio4(int vgar[G]){
	int i, r;
	FILE *arq1;
	char linha[80];
	
	printf("DISPONIBILIDADE DE VAGAS DE GARAGEM - AL2 PREMIUM RESIDENCE\n");
	printf("\nVaga\tVaga\tVaga\tVaga\tVaga\tVaga\tVaga\tVaga\tVaga\tVaga\n");
	for(i=0; i<G; i++){
		if(vgar[i]==0){
			printf("G%d\t", i+1);
		}
	}
	
	printf("\n\n1 - SALVAR EM ARQUIVO\n0 - RETORNAR AO MENU INICIAL\n");
	scanf("%d", &r);
	limpabuffer();
		switch(r){
			case 0:
				system("cls");
        	   	break;
			case 1:
				arq1=fopen("VagasDisponiveis.txt", "w");
				if(!arq1){
					printf("Erro na geracao do arquivo!");
				} else{
					fprintf(arq1, "DISPONIBILIDADE DE VAGAS DE GARAGEM - AL2 PREMIUM RESIDENCE\n", linha);
					fprintf(arq1,"\nVaga\n", linha);
					for(i=0;i<G;i++){
						if(vgar[i]==0){
							fprintf(arq1, "G%d\n", i+1);							
						}
					}
				}
				fclose(arq1);
				printf("\nARQUIVO GERADO COM SUCESSO!\n");
				system("PAUSE");
				break;
			default:
				system("cls");
           		printf("*POR FAVOR DIGITE UMA DAS OPCOES!*\n");
           		system("PAUSE");
           		break;		
		}
}

void relatorio5(TApe ape[X][Y][Z], int vgar[G]){
	int i,j,k,l,r;
	FILE *arq1;
	char linha[80];
	
	printf("\nVAGAS ADQUIRIDAS - AL2 PREMIUM RESIDENCE\n");
	printf("\nBl Und Vaga\tBl Und Vaga\tBl Und Vaga\tBl Und Vaga\tBl Und Vaga\n");
	for(i=0; i<X; i++){
		for(j=0; j<Y;j++){
			for(k=0;k<Z;k++){
				for(l=0;l<G;l++){
					if(ape[i][j][k].ngar>0 && vgar[l]>0 && (ape[i][j][k].und+ape[i][j][k].bloco-vgar[l]==0)){	
						printf("%d %d G%d\t", (vgar[l]-ape[i][j][k].und), (vgar[l]-ape[i][j][k].bloco), l+1);
					}
				}
			}
			printf("\n");
		}
		printf("\n");
	}
	printf("\n\n1 - SALVAR EM ARQUIVO\n0 - RETORNAR AO MENU INICIAL\n");
	scanf("%d", &r);
	limpabuffer();
		switch(r){
			case 0:
				system("cls");
        	   	break;
			case 1:
				arq1=fopen("VagasAdquiridas.txt", "w");
				if(!arq1){
					printf("Erro na geracao do arquivo!");
				} else{
					fprintf(arq1, "VAGAS ADQUIRIDAS - AL2 PREMIUM RESIDENCE\n", linha);
					fprintf(arq1,"\nBl Und Vaga\tBl Und Vaga\tBl Und Vaga\tBl Und Vaga\tBl Und Vaga\tBl Und Vaga\tBl Und Vaga\n", linha);
					for(i=0; i<X; i++){
						for(j=0; j<Y;j++){
							for(k=0;k<Z;k++){
								for(l=0;l<G;l++){
									if(ape[i][j][k].ngar>0 && vgar[l]>0 && (ape[i][j][k].und+ape[i][j][k].bloco-vgar[l]==0)){	
										fprintf(arq1, "%d %d G%d\t", (vgar[l]-ape[i][j][k].und), (vgar[l]-ape[i][j][k].bloco), l+1);
									}
								}
							}
							fprintf(arq1,"\n", linha);
						}
						fprintf(arq1,"\n", linha);
					}
				}
				fclose(arq1);
				printf("\nARQUIVO GERADO COM SUCESSO!\n");
				system("PAUSE");
				break;
			default:
				system("cls");
           		printf("*POR FAVOR DIGITE UMA DAS OPCOES!*\n");
           		system("PAUSE");
           		break;		
		}
	
}

void relatorio6(TApe ape[X][Y][Z]){
	int i, j, k, r, s, t, bl;
	char nomedoarquivo[20];
	FILE *arq1;
	char linha[80];
	
	printf("DIGITE O STATUS DESEJADO\n      0 - Disponivel\n      1 - Vendido\n      2 - Reservado\n      3 - Construtora\n");
	scanf("%d", &s);
	limpabuffer();
	if(s>=0 && s<=3){
		printf("\nDIGITE O TIPO DE RELATORIO\n      0 - Total\n      1 - Por bloco\n");
		scanf("%d", &t);
		limpabuffer();
		switch(t){
			case 0:
				if(s==0){
					printf("\nAPARTAMENTOS POR STATUS - TOTAL DISPONIVEIS - AL2 PREMIUM RESIDENCE\n");
				} else if(s==1){
					printf("\nAPARTAMENTOS POR STATUS - TOTAL VENDIDOS - AL2 PREMIUM RESIDENCE\n");
				} else if(s==2){
					printf("\nAPARTAMENTOS POR STATUS - TOTAL RESERVADOS - AL2 PREMIUM RESIDENCE\n");
				} else{
					printf("\nAPARTAMENTOS POR STATUS - TOTAL CONSTRUTORA - AL2 PREMIUM RESIDENCE\n");
				}
				printf("\nBl Und\tBl Und\tBl Und\tBl Und\tBl Und\tBl Und\n");
		
				for(i=0; i<X; i++){
					for(j=0; j<Y;j++){
						for(k=0; k<Z;k++){
							if(ape[i][j][k].status==s){
								printf("%d ", ape[i][j][k].bloco);
								printf("%d\t", ape[i][j][k].und);
							}
						}
						printf("\n");
					}
					printf("\n");
				}
				
				printf("\n1 - SALVAR EM ARQUIVO\n0 - RETORNAR AO MENU INICIAL\n");
				scanf("%d", &r);
				limpabuffer();
				switch(r){
					case 0:
						system("cls");
			           	break;
		
					case 1:
						if(s==0){
							strcpy(nomedoarquivo, "TotalDisponiveis.txt");
						} else if(s==1){
							strcpy(nomedoarquivo, "TotalVendidos.txt");
						} else if(s==2){
							strcpy(nomedoarquivo, "TotalReservados.txt");
						} else{
							strcpy(nomedoarquivo, "TotalConstrutora.txt");
						}
						
						arq1=fopen(nomedoarquivo, "w");
						if(!arq1){
							printf("Erro na geracao do arquivo!");
						} else{
								if(s==0){
									fprintf(arq1,"APARTAMENTOS POR STATUS - TOTAL DISPONIVEIS - AL2 PREMIUM RESIDENCE\n", linha);
								} else if(s==1){
									fprintf(arq1, "APARTAMENTOS POR STATUS - TOTAL VENDIDOS - AL2 PREMIUM RESIDENCE\n", linha);
								} else if(s==2){
									fprintf(arq1, "APARTAMENTOS POR STATUS - TOTAL RESERVADOS - AL2 PREMIUM RESIDENCE\n", linha);
								} else{
									fprintf(arq1, "APARTAMENTOS POR STATUS - TOTAL CONSTRUTORA - AL2 PREMIUM RESIDENCE\n", linha);
								}
								fprintf(arq1,"\nBl Und\tBl Und\tBl Und\tBl Und\tBl Und\tBl Und\n", linha);
								for(i=0; i<X; i++){
									for(j=0; j<Y;j++){
										for(k=0;k<Z;k++){
											if(ape[i][j][k].status==s){
												fprintf(arq1,"%d ", ape[i][j][k].bloco);
												fprintf(arq1,"%d\t", ape[i][j][k].und);
											}
										}
										fprintf(arq1,"\n", linha);
									}
									fprintf(arq1,"\n", linha);
								}
						}
						fclose(arq1);
						printf("\nARQUIVO GERADO COM SUCESSO!\n");
						system("PAUSE");								
						break;				
				}
				break;
								
			case 1:
				printf("\nDIGITE O BLOCO DESEJADO\n      1 - Array\n      2 - Matrix\n      3 - Struct\n      4 - File\n");
				scanf("%d", &bl);
				limpabuffer();
				if(bl==1){
					switch(s){
						case 0:
							strcpy(nomedoarquivo, "ArrayDisponiveis.txt");
							break;
						case 1:
							strcpy(nomedoarquivo, "ArrayVendidos.txt");
							break;
						case 2:
							strcpy(nomedoarquivo, "ArrayReservados.txt");
							break;
						case 3:
							strcpy(nomedoarquivo, "ArrayConstrutora.txt");
							break;
					}	
				} else if(bl==2){
					switch(s){
						case 0:
							strcpy(nomedoarquivo, "MatrixDisponiveis.txt");
							break;
						case 1:
							strcpy(nomedoarquivo, "MatrixVendidos.txt");
							break;
						case 2:
							strcpy(nomedoarquivo, "MatrixReservados.txt");
							break;
						case 3:
							strcpy(nomedoarquivo, "MatrixConstrutora.txt");
							break;
					}	
				} else if(bl==3){
					switch(s){
						case 0:
							strcpy(nomedoarquivo, "StructDisponiveis.txt");
							break;
						case 1:
							strcpy(nomedoarquivo, "StructVendidos.txt");
							break;
						case 2:
							strcpy(nomedoarquivo, "StructReservados.txt");
							break;
						case 3:
							strcpy(nomedoarquivo, "StructConstrutora.txt");
							break;
					}
				} else{
					switch(s){
						case 0:
							strcpy(nomedoarquivo, "FileDisponiveis.txt");
							break;
						case 1:
							strcpy(nomedoarquivo, "FileVendidos.txt");
							break;
						case 2:
							strcpy(nomedoarquivo, "FileReservados.txt");
							break;
						case 3:
							strcpy(nomedoarquivo, "FileConstrutora.txt");
							break;
					}
				}
								
				if(bl>0 && bl <5){
					if(s==0){
						printf("\nAPARTAMENTOS POR STATUS - BLOCO %d DISPONIVEIS - AL2 PREMIUM RESIDENCE\n", bl);
					} else if(s==1){
						printf("\nAPARTAMENTOS POR STATUS - BLOCO %d VENDIDOS - AL2 PREMIUM RESIDENCE\n", bl);
					} else if(s==2){
						printf("\nAPARTAMENTOS POR STATUS - BLOCO %d RESERVADOS - AL2 PREMIUM RESIDENCE\n", bl);
					} else{
						printf("\nAPARTAMENTOS POR STATUS - BLOCO %d CONSTRUTORA - AL2 PREMIUM RESIDENCE\n", bl);
					}
					printf("\nBl Und\tBl Und\tBl Und\tBl Und\tBl Und\tBl Und\n");
						
					for(j=0; j<Y;j++){
						for(k=0;k<Z;k++){
							if(ape[bl-1][j][k].status==s){
								printf("%d ", ape[bl-1][j][k].bloco);
								printf("%d\t", ape[bl-1][j][k].und);
							}
						}
						printf("\n");
					}
					printf("\n");
				} else{
					system("cls");
					printf("BLOCO INVALIDO!\n");
					system("PAUSE");					
				}
				
				printf("\n1 - SALVAR EM ARQUIVO\n0 - RETORNAR AO MENU INICIAL\n");
				scanf("%d", &r);
				limpabuffer();
				switch(r){
					case 0:
						system("cls");
			           	break;
			           	
			        case 1:
						arq1=fopen(nomedoarquivo, "w");
						if(!arq1){
							printf("Erro na geracao do arquivo!");
						} else{
							fprintf(arq1, "DISPONIBILIDADE DE APARTAMENTOS POR BLOCO - AL2 PREMIUM RESIDENCE\n", linha);
							fprintf(arq1,"\nBl Und\tBl Und\tBl Und\tBl Und\tBl Und\tBl Und\n", linha);
							for(j=0; j<Y;j++){
								for(k=0;k<Z;k++){
									if(ape[bl-1][j][k].status==s){
										fprintf(arq1,"%d ", ape[bl-1][j][k].bloco);
										fprintf(arq1,"%d\t", ape[bl-1][j][k].und);
									}
								}
								fprintf(arq1,"\n", linha);
							}			
						}
						fclose(arq1);
						printf("\nARQUIVO GERADO COM SUCESSO!\n");
						system("PAUSE");				
						break;
				
					default:
						{
						system("cls");
        				printf("*OPCAO NAO EXISTENTE!\n*");
        				system("PAUSE");					   	     		
						}				
				}
				break;
				
			default:
				{
				system("cls");
				printf("OPCAO INVALIDA!\n");
				system("PAUSE");	
				}
		}
	} else{
		system("cls");
		printf("STATUS INVALIDO!\n");
		system("PAUSE");
	}	
}

void relatorio7(TApe ape[X][Y][Z]){
	int i, j, k, l, mes, ano, r;
	float tcomissao[F];
	char nomedoarquivo[60]={"ComissaoVendedores"}; 
	char month[12][10]={"Janeiro","Fevereiro", "Marco", "Abril", "Maio", "Junho", "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"};
	char txt[5]={".txt"};
	char year[5];
	
	FILE *arq1;
	char linha[80];

	zerafloat(tcomissao);
	printf("\nDIGITE O MES(MM): ");
	scanf("%d", &mes);
	limpabuffer();
	strcat(nomedoarquivo,month[mes-1]);
	printf("\nDIGITE O ANO(AAAA): ");
	scanf("%d", &ano);
	limpabuffer();
	itoa(ano,year,10);
	strcat(nomedoarquivo,year);
	strcat(nomedoarquivo, txt);

	for(i=0;i<X;i++){
		for(j=0; j<Y;j++){
			for(k=0;k<Z;k++){
				for(l=0; l<F; l++){
					if((mes==ape[i][j][k].vendedor.data.mes) && (ano==ape[i][j][k].vendedor.data.ano)){
						if(l==ape[i][j][k].vendedor.codigo-1){
							tcomissao[l]+=ape[i][j][k].comissao;
						}
					}
				}
			}
		}
	}
	printf("\nCOMISSAO DOS VENDEDORES - AL2 PREMIUM RESIDENCE\n");
	printf("\tCOMPETENCIA %d/%d\n\n", mes, ano);
	for(i=0; i<F; i++){
		printf("%d\t %.2f\n", i+1, tcomissao[i]);
	}
	
	printf("\n\n1 - SALVAR EM ARQUIVO\n0 - RETORNAR AO MENU INICIAL\n");
	scanf("%d", &r);
	limpabuffer();
		switch(r){
			case 0:
				system("cls");
        	   	break;
			case 1:
				arq1=fopen(nomedoarquivo, "w");
				if(!arq1){
					printf("Erro na geracao do arquivo!");
				} else{
					fprintf(arq1, "COMISSAO DOS VENDEDORES - AL2 PREMIUM RESIDENCE\n", linha);
					fprintf(arq1, "\tCOMPETENCIA %d/%d\n", mes, ano);
					fprintf(arq1,"\nCodigo\tComissao\n", linha);
					for(i=0; i<F; i++){
						fprintf(arq1, "%d\t %.2f\n", i+1, tcomissao[i]);
					}
				}
				fclose(arq1);
				printf("\nARQUIVO GERADO COM SUCESSO!\n");
				system("PAUSE");
				break;
			default:
				system("cls");
           		printf("*POR FAVOR DIGITE UMA DAS OPCOES!*\n");
           		system("PAUSE");
           		break;		
		}
}

void relatorio8(TApe ape[X][Y][Z]){
	int i, j, k, l, r, codv;
	char nomedoarquivo[60]={"Reservas"};
	char txt[5]={".txt"};
	FILE *arq1;
	char linha[80];
	
	printf("\nDIGITE O CODIGO DO VENDEDOR: ");
	scanf("%d", &codv);
	limpabuffer();
	printf("\nRESERVA DOS CLIENTES POR VENDEDOR - AL2 PREMIUM RESIDENCE\n");
	printf("\nBl Und\tCPF\t\tTelefone\tNome\n");
	
	for(i=0;i<X;i++){
		for(j=0; j<Y;j++){
			for(k=0;k<Z;k++){
				if(ape[i][j][k].status==2){
					if(codv==ape[i][j][k].vendedor.codigo){
						strcat(nomedoarquivo,ape[i][j][k].vendedor.nome);
						strcat(nomedoarquivo,txt);
						printf("%d ", ape[i][j][k].bloco);
						printf("%d\t", ape[i][j][k].und);						
						printf("%.0f\t", ape[i][j][k].cliente.cpf);
						printf("%.0f\t", ape[i][j][k].cliente.telefone);
						printf("%s\n", ape[i][j][k].cliente.nome);
					}
				}
			}
		}
	}

	printf("\n\n1 - SALVAR EM ARQUIVO\n0 - RETORNAR AO MENU INICIAL\n");
	scanf("%d", &r);
	limpabuffer();
		switch(r){
			case 0:
				system("cls");
        	   	break;
			case 1:
				arq1=fopen(nomedoarquivo, "w");
				if(!arq1){
					printf("Erro na geracao do arquivo!");
				} else{
					fprintf(arq1, "RESERVA DOS CLIENTES POR VENDEDOR - AL2 PREMIUM RESIDENCE\n", linha);
					fprintf(arq1, "\nBl Und\tCPF\t\tTelefone\tNome\n", linha);
					for(i=0;i<X;i++){
						for(j=0; j<Y;j++){
							for(k=0;k<Z;k++){
								if(ape[i][j][k].status==2){
									if(codv==ape[i][j][k].vendedor.codigo){
										fprintf(arq1, "%d ", ape[i][j][k].bloco);
										fprintf(arq1, "%d\t", ape[i][j][k].und);										
										fprintf(arq1, "%.0f\t", ape[i][j][k].cliente.cpf);
										fprintf(arq1, "%.0f\t", ape[i][j][k].cliente.telefone);
										fprintf(arq1, "%s\n", ape[i][j][k].cliente.nome);	
									}
								}
							}
						}
					}
				}
				fclose(arq1);
				printf("\nARQUIVO GERADO COM SUCESSO!\n");
				system("PAUSE");
				break;
			default:
				system("cls");
           		printf("*POR FAVOR DIGITE UMA DAS OPCOES!*\n");
           		system("PAUSE");
           		break;		
		}	
}

//Função principal

int main(){																				//Início do programa
	char nomedoarquivo[50]={"origem.bin"};
	char nomedoarquivo2[50]={"atual.bin"};
	char nomedoarquivo3[50]={"garagem.bin"};
	TApe ape[X][Y][Z];
	int i, j, k, rmenu, vcadastro, pgar, ngar, qgar, bl, und, r, at, atg, flag=0;
	int mod;
	int vgar[G];	
	int tarq, tarq2, tarqg;	
	FILE *arq1;
	
	arq1=fopen("atual.bin", "rb");
	if(!arq1){
		tarq = origem(ape, nomedoarquivo);
		zeravet(vgar);
	}else{
		tarq2 = lebin(ape, nomedoarquivo2);
		tarqg = lebing(vgar, nomedoarquivo3);
	}
	
		
	while(flag==0 && rmenu!=9){
		rmenu=menu();
		switch(rmenu){
			case 1:
				system("cls");
    			printf("\nCADASTRO DE VENDAS E RESERVAS\n\n");
    			printf("APARTAMENTO DESEJADO\n\n");
    			printf("   BLOCO\n      1 - Array\n      2 - Matrix\n      3 - Struct\n      4 - File\n");
				scanf("%d", &bl);
    			limpabuffer();
				printf("\n");
    			if(bl>0 && bl <5){
					printf("   UNIDADE\n");
    				printf("      Cada bloco possui 10 pavimentos.\n");
    				printf("      Cada pavimento possui 6 apartamentos.\n");
					printf("         Ex.: 306 representa o sexto apartamento do pavimento 3.\n");
					printf("           As unidades terminadas em 2 e 5 possuem 3 quartos.\n");
					printf("           As unidades terminadas em 1 e 4 possuem 2 quartos.\n");
					printf("           As unidades terminadas em 3 e 6 possuem 1 quarto.\n");
    				scanf("%d", &und);
					limpabuffer();   	
    				printf("\n");
    			
					if((und>100 && und<107) || (und>200 && und<207) || (und>300 && und<307) || (und>400 && und<407) || (und>500 && und<507) || (und>600 && und<607) || (und>700 && und<707) || (und>800 && und<807) || (und>900 && und<907) || (und>1000 && und<1007)){
    					printf("   MODALIDADE\n      1 - Compra\n      2 - Reserva\n      3 - Construtora\n      0 - Tornar disponivel\n");
    					scanf("%d", &mod);
    					limpabuffer();
						printf("\n");
				
						if(ape[bl-1][und/100-1][und%100-1].status==0 || mod==0){
							switch(mod){
								case 1:
									ape[bl-1][und/100-1][und%100-1].status=mod;										//Guarda o status do apartamento (1 Compra)
									cliente(ape,bl,und);
									vendedor(ape,bl,und);
									pgar=pgaragem(ape,bl,und);
									if(pgar>0){
										ngar=ngaragem(ape,bl,und);
										garagem(ape, bl, und, vgar, ngar);
									}	
									tcompra(ape,bl,und);
									comissao(ape,bl,und);
													   		
 		   							printf("\nO apartamento foi vendido para o cliente.\n");
									system("PAUSE");
									system("cls");
									break;
				
								case 2:   
									ape[bl-1][und/100-1][und%100-1].status=mod;										//Guarda o status do apartamento (2 Reserva)
									ape[bl-1][und/100-1][und%100-1].tinicio=time(NULL);								
						 
							 		cliente(ape,bl,und);
									vendedor(ape,bl,und);
						 		   		 		   		
 		   							printf("O apartamento foi reservado para o cliente.\n");
									system("PAUSE");
									system("cls"); 		   			
									break;
					
								case 3:
									ape[bl-1][und/100-1][und%100-1].status=mod;										//Guarda o status do apartamento (3 Construtora)
														
									printf("O apartamento foi reservado para a construtora.\n");
									system("PAUSE");
									system("cls"); 		   						
 		   							break;
 		   			
 		   						case 0:
 		   							ape[bl-1][und/100-1][und%100-1].status=mod;										//Guarda o status do apartamento (0 Disponível)
 		   													
									printf("O apartamento voltou a estar disponivel.\n");
									system("PAUSE");
									system("cls"); 		   						
 		   							break;
 		   		
		 			   			default:
 					   				{
									system("cls");
        							printf("*MODALIDADE NAO EXISTENTE!\n*");
        							system("PAUSE");					   	     		
									}
							}
						} else{
							system("cls");
        					printf("*UNIDADE OCUPADA!\n*");
        					system("PAUSE");        				
						}
					} else{
						system("cls");
        				printf("*UNIDADE NAO EXISTENTE!\n*");
        				system("PAUSE");        				
					}    	   	
		    	} else{
    				system("cls");
        			printf("*BLOCO NAO EXISTENTE!\n*");
        			system("PAUSE");
				}    
				break;
			case 2:
				system("cls");
				r=menu_relatorio(ape);
				system("cls");
				switch(r){
					case 1:
						relatorio1(ape);
						system("cls");
						break;
					case 2:
						relatorio2(ape);
						system("cls");
						break;
					case 3:
						relatorio3(ape);
						system("cls");
						break;
					case 4:
						relatorio4(vgar);
						system("cls");
						break;
					case 5:
						relatorio5(ape, vgar);
						system("cls");
						break;
					case 6:
						relatorio6(ape);
						system("cls");
						break;
					case 7:
						relatorio7(ape);
						system("cls");
						break;
					case 8:
						relatorio8(ape);
						system("cls");
						break;
					case 9:
						system("cls");
						break;			
					default:
						{
						system("cls");
        				printf("*OPCAO NAO EXISTENTE!\n*");
        				system("PAUSE");					   	     		
						}	
				}
				break;
		
			case 9:
				flag==1;			
				break;
			
			default:
		    	{
            	system("cls");
           		printf("*POR FAVOR DIGITE UMA DAS OPCOES!*");
				break;
            	}
		}
	}
	at=atual(ape, nomedoarquivo2);
	atg=atualg(vgar, nomedoarquivo3);
	return 0;
}

void limpabuffer(){
	char c;
	while((c=getchar())&&c!=EOF&&c!='\n');
	setbuf(stdin,NULL);
}
