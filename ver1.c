#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void* vtable;
} Object;


typedef struct {
    void* vtable;
} Greeter;

void Greeter_greet(Greeter* this) {
    printf("Hello!\n");
}

typedef struct {
    void (*greet)(Greeter* this);
} GreeterVTable;

static GreeterVTable Greeter_vtable = {
    .greet = Greeter_greet,
};

Greeter* new_Greeter() {
    Greeter* instance = (Greeter*) malloc(sizeof(Greeter));
    instance->vtable = (void*) &Greeter_vtable;
    return instance;
}


typedef struct {
    void* vtable;
    char username[255];
} UsernameGreeter;

void UsernameGreeter_greet(UsernameGreeter* this) {
    printf("Hello %s!\n", this->username);
}

void UsernameGreeter_set_username(UsernameGreeter* this, const char* username) {
    strcpy(this->username, username);
}

typedef struct {
    void (*greet)(UsernameGreeter* this);
    void (*set_username)(UsernameGreeter* this, const char* username);
} UsernameGreeterVTable;

static UsernameGreeterVTable UsernameGreeter_vtable = {
    .greet = UsernameGreeter_greet,
    .set_username = UsernameGreeter_set_username,
};

UsernameGreeter* new_UsernameGreeter() {
    UsernameGreeter* instance = (UsernameGreeter*) malloc(sizeof(UsernameGreeter));
    instance->vtable = (void*) &UsernameGreeter_vtable;
    return instance;
}


void test_greeter() {
    Greeter* greeter = new_Greeter();
    ((GreeterVTable*)(greeter->vtable))->greet(greeter);
    free(greeter);
}


void test_username_greeter() {
    UsernameGreeter* greeter = new_UsernameGreeter();
    ((UsernameGreeterVTable*)(greeter->vtable))->set_username((UsernameGreeter*) greeter, "Dmitry");
    ((UsernameGreeterVTable*)(greeter->vtable))->greet((UsernameGreeter*) greeter);
    
    ((GreeterVTable*)(greeter->vtable))->greet((Greeter*) greeter);
    
    free(greeter);
}


#define VCALL(obj, type, method_name, ...) ((type##VTable*)(obj->vtable))->method_name((type*) obj, ##__VA_ARGS__)


void test_vcall() {
    UsernameGreeter* greeter = new_UsernameGreeter();    
    VCALL(greeter, UsernameGreeter, set_username, "Username");
    VCALL(greeter, Greeter, greet);
    free(greeter);
}


int main(int argc, char** argv) {
    test_greeter();
    test_username_greeter();
    test_vcall();
    printf("Success\n");
    return 0;
}
