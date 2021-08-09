#include<fcntl.h>
#include<stdio.h>
#include<sys/wait.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

char MENSAJE_ERROR[50] = "Se ha presentado un error";
int MultipathNumero=0;
char multiPath[512][512];
int sizeLote= 512;
int cambioPath=0;
int CLOSED=0;
int pathVacio=0;
char *path;
int lote=0;

//Imprime repetidamente el prompt wish 

//Analiza la entrada y luego ejecuta el comando especificado.
//Este proceso se repite indefinidamente hasta que algún usuario escriba exit. 
void promptPpal(){
        write(STDOUT_FILENO, "wish> ", strlen("wish> "));                
}

int ComprobarEspacio(char* buffer){
        int bandera=0;
        for(int i=0;i<strlen(buffer);i++){
                if(isspace(buffer[i])==0){
                        bandera=1;
                        break;
                }
        }        
        return bandera;
}  


void impresionError(){
        write(STDERR_FILENO, MENSAJE_ERROR, strlen(MENSAJE_ERROR));
        exit(1);
        
}

int ProcesoNuevo(char *myargs[]) {
        int np=fork(); //Función Fork realiza copias exactas de un repositorio para ser usadas en cualquier instancia
        if(np<0){ //Lineas realizadas por si se presenta un error en el Fork
                impresionError();
                exit(1); 
        }
        else if(np==0 && pathVacio!=1){ //Proceso que se realiza para la revisión del método
                if(cambioPath==0){
                        path=strdup("/bin/");
                        path=strcat(path,myargs[0]); // Se revisa si el acceso fue correcto o no
                        if(access(path,X_OK)!=0 && cambioPath==0){ 
                                path=strdup("/usr/bin/");
                                path=strcat(path,myargs[0]);
                                if(access(path,X_OK)!=0){
                                        write(STDERR_FILENO, MENSAJE_ERROR, strlen(MENSAJE_ERROR));
                                        exit(0);
                                }
                        }             
                  }
                  else if(cambioPath==1 && MultipathNumero==0){   //Verifica las opciones de CambioPath y Numero De Multipath para ver si actuar o lanzar mensaje error
                         path=strcat(path,myargs[0]);
                        if(access(path,X_OK)!=0){
                                        write(STDERR_FILENO, MENSAJE_ERROR, strlen(MENSAJE_ERROR));
                                        exit(0);
                                }
                  }
                  else{
                        for(int x=0;x<MultipathNumero;x++){              //Opción que se realiza si las otras no se cumplen.
                                strcat(multiPath[x],myargs[0]);
                                 if(access(multiPath[x],X_OK)==0){
                                        strcpy(path,multiPath[x]);
                                        break;
                                }
                        }
                        
                  }
                  if(execv(path,myargs)==-1){    // Revisa si se ejecuto correctamente o no
                                impresionError();
                                exit(0);
                  }
                  
        }
        else {
                int estado=0;
        }
        return np;
}



int procesamiento_anterior(char *buffer){
        int stdout_copy=0;
        int np;
        if(strstr(buffer,">")!=NULL){   // Método de redireccionamiento
                        int b=0;        // Cuando un usuario quiere enviar la salida de un programa a un archivo en lugar de la pantalla
                        
                        char* multiRedirect[sizeof(char)*512];
                        multiRedirect[0]= strtok(strdup(buffer)," \n\t>");
                        while(multiRedirect[b]!=NULL){
                                b++;
                                multiRedirect[b]=strtok(NULL," \n\t>");
                        }
                        if(b==1){    //Mensaje de error cuando no hay archivo donde enviarlo 
                            write(STDERR_FILENO, MENSAJE_ERROR, strlen(MENSAJE_ERROR)); 
                            exit(0);    
                        }
                        int i=0;
                        char* myargs[sizeof(buffer)];
                        myargs[0]= strtok(buffer,"\n\t>");
                        while(myargs[i]!=NULL){
                                i++;
                                myargs[i]=strtok(NULL," \n\t>"); 
                        }
                        if(i>2){    //Mensaje de error cuando no hay archivo donde enviarlo 
                            write(STDERR_FILENO, MENSAJE_ERROR, strlen(MENSAJE_ERROR)); 
                            exit(0);    
                        }
                        int x=0;
                        char* tokenize[sizeof(myargs[1])];
                        tokenize[0]= strtok(myargs[1]," \n\t");
                        while(tokenize[x]!=NULL){
                                x++;
                                tokenize[x]=strtok(NULL," \n\t"); 
                        }
                        
                        char *fout=strdup(tokenize[0]);
                        stdout_copy=dup(1);
                        int out=open(fout,O_WRONLY|O_CREAT|O_TRUNC,0666);
                        int error=open(fout,O_WRONLY|O_CREAT|O_TRUNC,0666);
                        fflush(stdout);
                        dup2(out,STDOUT_FILENO);
                        dup2(out,STDERR_FILENO);
                        close(out);
                        CLOSED=1;
                        if(out==-1 || error==-1 || x>1 || i>2){
                                write(STDERR_FILENO, MENSAJE_ERROR, strlen(MENSAJE_ERROR));
                                exit(0);
                        }
                        myargs[i+1]=NULL;
                        tokenize[x+1]=NULL;
                        strcpy(buffer,myargs[0]);
                        
                        
                }
                
                if(buffer[0] != '\0' && buffer[0] != '\n') {
                        char *command[sizeof(buffer)];
                        command[0] = strtok(buffer, " \t\n");
                        int p=0;
                        while(command[p]!=NULL){
                                p++;
                                command[p]=strtok(NULL, " \n\t");
                                
                        }
                        command[p+1]=NULL;
		        if(strcmp(command[0],"cd") == 0){//cd
                                if(p==2){
                                        if(chdir(command[1])!=0){
                                                write(STDERR_FILENO, MENSAJE_ERROR, strlen(MENSAJE_ERROR));
                                               
                                        }
                                 }
                                 else{ // Error cuando se presentan 0 argumentos para revisar o más de dos argumentos que no es válido
                                        write(STDERR_FILENO, MENSAJE_ERROR, strlen(MENSAJE_ERROR));
                                        
                                 }
                                                        
                        }  
                        else if(strcmp(command[0],"path") == 0){
                                cambioPath=1;
                                if(p==2){
                                        pathVacio=0;
                                        path=strdup(command[1]);
                                         if(path[strlen(path)-1]!='/'){
                                                strcat(path,"/");
                                        }      
                                }
                                else if(p==1){
                                       
                                        pathVacio=1;
                                }
                                else{ 
                                        pathVacio=0;
                                        for(int i=1;i<p;i++){
                                                char *temp=strdup(command[i]);
                                                if(temp[strlen(temp)-1]!='/')
                                                        strcat(temp,"/");
                                                strcpy(multiPath[i-1],temp);
                                                MultipathNumero++;
                                        }

                        
                                }
                                
                                       
			}
                        else if(strcmp(command[0],"exit") == 0) {
			    if(p==1){
                                        exit(0);
                                }
                                else{ // Error cuando se presentan 0 argumentos para revisar o más de dos argumentos que no es válido
                                        write(STDERR_FILENO, MENSAJE_ERROR, strlen(MENSAJE_ERROR));
                                        
                                }
                        }    
                        else{
                                if(pathVacio==1)
                                        write(STDERR_FILENO, MENSAJE_ERROR, strlen(MENSAJE_ERROR));
                                else
                                        np=ProcesoNuevo(command);
                        }

                }
                if(CLOSED==1){
                        dup2(stdout_copy,1);
                        close(stdout_copy);
                        
                }
               return np;
}



int main(int argc, char* argv[]){
        FILE *file = NULL;
        path=(char*) malloc(sizeLote);
        char buffer[sizeLote];
        
        
        
        if(argc==1){ // Modo diferente al de los lotes. 
                file=stdin; //Guardado estándar stdin para la linea de entrada
                promptPpal();
        }
        
        else if(argc==2){ //Parte del código del modo por lotes, donde el shell recibe un archivo de entrada de comando
                
                char *bFile= strdup(argv[1]);   //Abrir archivos para que los comandos se ejecuten.
                file = fopen(bFile, "r");
                if (file == NULL) {
        	        impresionError();
                }
                lote=1;
        }
        else{
                impresionError();
        }

        while(fgets(buffer, sizeLote, file)){ //Escribe desde el archivo hasta el Buffer
                CLOSED=0;
                if(ComprobarEspacio(buffer)==0){ //Verifica si el Buffer tiene espacio libre
                        continue;
                }
                if(strstr(buffer,"&")!=NULL){//Manejo de la concurrencia
                        int j=0;
                        char *myargs[sizeof(buffer)];
                        myargs[0]= strtok(buffer,"\n\t&");                            
                        while(myargs[j]!=NULL){
                                j++;
                                myargs[j]=strtok(NULL,"\n\t&"); 
                                                  
                                
                        }
                        myargs[j+1]=NULL;
                        int pid[j];
                        for(int i=0;i<j;i++){
                                pid[i]=procesamiento_anterior(myargs[i]);
                                
                        for(int x=0;x<j;i++){
                                int returnStatus=0;
                                waitpid(pid[x],&returnStatus,0);                        
                                if (returnStatus == 1)      
                                {
                                        impresionError();    
                                }
                        
                        }
                      }
                }
                else{
                    	procesamiento_anterior(buffer);
                }
                if(argc == 1) {
                        promptPpal();
                 }
   
        }
}
        

