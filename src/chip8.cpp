#include <chip8.h>

unsigned char font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, 
    0x20, 0x60, 0x20, 0x20, 0x70, 
    0xF0, 0x10, 0xF0, 0x80, 0xF0, 
    0xF0, 0x10, 0xF0, 0x10, 0xF0, 
    0x90, 0x90, 0xF0, 0x10, 0x10, 
    0xF0, 0x80, 0xF0, 0x10, 0xF0, 
    0xF0, 0x80, 0xF0, 0x90, 0xF0, 
    0xF0, 0x10, 0x20, 0x40, 0x40, 
    0xF0, 0x90, 0xF0, 0x90, 0xF0, 
    0xF0, 0x90, 0xF0, 0x10, 0xF0, 
    0xF0, 0x90, 0xF0, 0x90, 0x90, 
    0xE0, 0x90, 0xE0, 0x90, 0xE0, 
    0xF0, 0x80, 0x80, 0x80, 0xF0, 
    0xE0, 0x90, 0x90, 0x90, 0xE0, 
    0xF0, 0x80, 0xF0, 0x80, 0xF0, 
    0xF0, 0x80, 0xF0, 0x80, 0x80
};

Chip8::Chip8(){
    pc = 0x200;
    drawFlag = false;

    srand (time(NULL));

    for(int i = 0; i<2048; i++)
        gfx[i] = 0;

    delay_timer = 0;
    sound_timer = 0;

    for(int i = 0; i<4096; i++)
        memory[i] = 0;

    for(int i = 0; i < 80; i++)
        memory[i] = font[i];

    for(int i = 0; i<16; i++) {
        stack[i] = 0;
        V[i] = 0;
        key[i] = 0;
    }
    
    sp = 0;
}

Chip8::~Chip8(){

}

int Chip8::LoadGame(std::string game){
    std::ifstream file;
    file.open(game, std::ios::in | std::ios::binary | std::ios::ate);

    if(!file.is_open()){
        std::cout << "Unable to open game rom." << std::endl;
        return 1;
    }

    file.seekg(0, std::ios::end);
    int size = file.tellg();
    char *data = nullptr;
    data = new char[size];
    file.seekg(0, std::ios::beg);
    file.read(data, size);

    for(int i = 0; i < size; i++){
        memory[i + 512] = data[i];
    }
    
    file.close();
    delete[] data;

    return 0;
}

bool Chip8::shouldDraw(){
    return drawFlag;
}

uint8_t *Chip8::getPixel(){
    return gfx;
}

void Chip8::keyPressed(int key, bool pressed){
    this->key[key] = pressed ? 1 : 0;
}

void Chip8::emulateCycle(){

    opcode = (memory[pc] << 8) | (memory[pc+1]);

    //std::cout << "0x" << std::hex << std::uppercase << opcode << std::nouppercase << std::dec << std::endl;
    unsigned short vx = (opcode & 0x0F00) >> 8;
    unsigned short vy = (opcode & 0x00F0) >> 4;

    switch(opcode & 0xF000){
        case 0x0000:
            switch(opcode){
                case 0x00E0:
                    for(int i = 0; i<2048; i++)
                        gfx[i] = 0;
                    
                    drawFlag = true;
                    pc += 2;

                    break;
                case 0x00EE:
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;

                default:
                    std::cout << std::dec << opcode << std::endl;
                    exit(1);
                    break;
                }

            break;
        

        case 0x1000:
            pc = opcode & 0x0FFF;
            break;

        case 0x2000:
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;

        case 0x3000:
            if(V[vx] == (opcode & 0x00FF))
                pc += 2;
            pc += 2;
            break;

        case 0x4000:
            if(V[vx] != (opcode & 0x00FF))
                pc += 2;
            pc += 2;
            break;

        case 0x5000:
            if(V[vx] == V[vy])
                pc += 2;
            pc += 2;
            break;

        case 0x6000:
            V[vx] = opcode & 0x00FF;
            pc += 2;
            break;

        case 0x7000:
            V[vx] += (opcode & 0x00FF);
            pc += 2;
            break;

        case 0x8000:
            switch(opcode & 0x000F){
                case 0x0000:
                    V[vx] = V[vy];
                    pc += 2;
                    break;

                case 0x0001:
                    V[vx] |= V[vy];
                    pc += 2;
                    break;

                case 0x0002:
                    V[vx] &= V[vy];
                    pc += 2;
                    break;

                case 0x0003:
                    V[vx] ^= V[vy];
                    pc += 2;
                    break;

                case 0x0004:
                    V[vx] += V[vy];
                    if(V[vy] > (0xFF - V[vx]))
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    pc += 2;
                    break;

                case 0x0005:
                    if(V[vy] > V[vx])
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;
                    V[vx] -= V[vy];  
                    pc += 2;
                    break;

                case 0x0006:
                    V[0xF] = V[vx] & 0x01;                    
                    V[vx] >>= 1;
                    pc += 2;
                    break;
                
                case 0x0007:
                    if(V[vx] > V[vy])
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;
                    V[vx] = V[vy] - V[vx];   
                    pc += 2;
                    break;
                
                case 0x000E:{
                    V[0xF] = V[vx] >> 7;
                    V[vx] <<= 1;
                    pc += 2;
                    break;
                }
                default:
                    std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
                    exit(1);
                    break;
            }
            break;

        case 0x9000:
            if(V[vx] != V[vy])
                pc += 2;
            pc += 2;
            break;

        case 0xA000:
            I = opcode & 0x0FFF;
            pc += 2;
            break;

        case 0xB000:
            pc = (opcode & 0x0FFF) + V[0];
            break;
        
        case 0xC000:
            V[vx] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
            pc += 2;
            break;

        case 0xD000: {
            unsigned short x = V[vx];
            unsigned short y = V[vy];
            unsigned short h = opcode & 0x000F;
            unsigned short p;

            V[0xF] = 0;
            for(int vertical = 0; vertical < h; vertical++){
                p = memory[I + vertical];
                for(int horizontal = 0; horizontal < 8; horizontal++){
                    if((p & (0x80 >> horizontal)) != 0){
                        if(gfx[(x + horizontal + ((vertical + y) * 64))] == 1)
                            V[0xF] = 1;
                        gfx[x + horizontal + ((vertical + y) * 64)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            pc += 2;
            break;
        }

        case 0xE000:
            switch(opcode & 0x00FF){
                case 0x009E:
                    if(key[V[vx]] != 0)
                        pc += 2;
                    pc += 2;
                    break;

                case 0x00A1:
                    if(key[V[vx]] == 0)
                        pc += 2;
                    pc += 2;                
                    break;
                default:
                    std::cout << "Unknown Opcode: " << std::hex << opcode << std::endl;
                    exit(1);
                    break;
            }

            break;

        case 0xF000:
            switch(opcode & 0x00FF){
                case 0x0007:
                    V[vx] = delay_timer;
                    pc += 2;
                    break;

                case 0x000A:{
                    bool pressed = false;
                    for(int i = 0; i < 16; ++i){
                        if(key[i] != 0){
                            V[vx] = i;
                            pressed = true;
                        }
                    }

                    if(!pressed)
                        return;

                    pc += 2;
                    break;
                }

                case 0x0015:
                    delay_timer = V[vx];
                    pc += 2;
                    break;

                case 0x0018:
                    sound_timer = V[vx];
                    pc += 2;
                    break;

                case 0x001E:
                    if(I + V[vx] > 0xFFF)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    I += V[vx];
                    pc += 2;
                    break;

                case 0x0029:
                    I = V[vx] * 0x5;
                    pc += 2;
                    break;

                case 0x0033:
                    memory[I] = V[vx] / 100;
                    memory[I + 1] = (V[vx] / 10) % 10;
                    memory[I + 2] = V[vx] % 10;
                    pc += 2;
                    break;

                case 0x0055:{
                    for(int i = 0; i <= vx; ++i)
                        memory[I + i] = V[i];
                    I += vx + 1;
                    pc += 2;
                }
                case 0x0065:{
                    for(int i = 0; i <= vx; ++i)
                        V[i] = memory[I + i];
                    I += vx + 1;    
                    pc += 2;
                    break;
                }
                default:
                    std::cout << "Unknown Opcode: " << std::hex << opcode << std::endl;
                    exit(1);
                    break;
            }

            break;

        default:
            std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
            exit(1);
            break;
    }


    if(delay_timer > 0)
        --delay_timer;
    
    if(sound_timer > 0){
        if(sound_timer == 1){
            //beep?
        }
        --sound_timer;
    }

}