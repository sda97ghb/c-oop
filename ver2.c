#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define VCALL(obj, type, method_name, ...) ((type##VTable*)(obj->vtable))->method_name((type*) obj, ##__VA_ARGS__)


typedef struct {
    void* vtable;
} Object;


// class Greeter extends Object

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
    .greet =
        (void (*)(Greeter* this))
        Greeter_greet,
};

Greeter* new_Greeter() {
    Greeter* instance = (Greeter*) malloc(sizeof(Greeter));
    instance->vtable = (void*) &Greeter_vtable;
    return instance;
}


// class MessageGreeter extends Greeter

typedef struct {
    void* vtable;
    char message[255];
} MessageGreeter;

void MessageGreeter_greet(MessageGreeter* this) {
    printf("%s\n", this->message);
}

void MessageGreeter_set_message(MessageGreeter* this, const char* message) {
    strcpy(this->message, message);
}

const char* MessageGreeter_get_message(MessageGreeter* this) {
    return this->message;
}

typedef struct {
    void (*greet)(MessageGreeter* this);
    void (*set_message)(MessageGreeter* this, const char* message);
    const char* (*get_message)(MessageGreeter* this);
} MessageGreeterVTable;

static MessageGreeterVTable MessageGreeter_vtable = {
    .greet =
        (void (*)(MessageGreeter* this))
        MessageGreeter_greet,
    .set_message =
        (void (*)(MessageGreeter* this, const char* message))
        MessageGreeter_set_message,
    .get_message =
        (const char* (*)(MessageGreeter* this))
        MessageGreeter_get_message,
};

MessageGreeter* new_MessageGreeter() {
    MessageGreeter* instance = (MessageGreeter*) malloc(sizeof(MessageGreeter));
    instance->vtable = (void*) &MessageGreeter_vtable;
    return instance;
}


// class UsernameGreeter extends MessageGreeter

typedef struct {
    void* vtable;
    char message[255];
    char username[255];
} UsernameGreeter;

void UsernameGreeter_set_username(UsernameGreeter* this, const char* username) {
    strcpy(this->username, username);
    char new_message[255];
    sprintf(new_message, "Hello %s!", username);
    VCALL(this, MessageGreeter, set_message, new_message);
}

typedef struct {
    void (*greet)(UsernameGreeter* this);
    void (*set_message)(UsernameGreeter* this, const char* message);
    const char* (*get_message)(UsernameGreeter* this);
    void (*set_username)(UsernameGreeter* this, const char* username);
} UsernameGreeterVTable;

static UsernameGreeterVTable UsernameGreeter_vtable = {
    .greet =
        (void (*)(UsernameGreeter* this))
        MessageGreeter_greet,
    .set_message =
        (void (*)(UsernameGreeter* this, const char* message))
        MessageGreeter_set_message,
    .get_message =
        (const char* (*)(UsernameGreeter* this))
        MessageGreeter_get_message,
    .set_username =
        (void (*)(UsernameGreeter* this, const char* username))
        UsernameGreeter_set_username,
};

UsernameGreeter* new_UsernameGreeter() {
    UsernameGreeter* instance = (UsernameGreeter*) malloc(sizeof(UsernameGreeter));
    instance->vtable = (void*) &UsernameGreeter_vtable;
    return instance;
}


void test_greeter() {
    Greeter* greeter = new_Greeter();
    VCALL(greeter, Greeter, greet);
    free(greeter);
}


void test_message_greeter() {
    MessageGreeter* greeter = new_MessageGreeter();
    VCALL(greeter, MessageGreeter, set_message, "Hi!");
    VCALL(greeter, MessageGreeter, greet);
    VCALL(greeter, Greeter, greet);
    free(greeter);
}

void test_username_greeter() {
    UsernameGreeter* greeter = new_UsernameGreeter();
    VCALL(greeter, UsernameGreeter, set_username, "Dmitry");
    VCALL(greeter, UsernameGreeter, greet);
    VCALL(greeter, MessageGreeter, greet);
    VCALL(greeter, Greeter, greet);
    free(greeter);
}


int main(int argc, char** argv) {
    test_greeter();
    test_message_greeter();
    test_username_greeter();
    printf("Success\n");
    return 0;
}
