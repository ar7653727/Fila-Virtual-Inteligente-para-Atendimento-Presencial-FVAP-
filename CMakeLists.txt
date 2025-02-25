# == Configuração Automática do VS Code para Raspberry Pi Pico ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# =====================================================================

# Versão mínima do CMake necessária
cmake_minimum_required(VERSION 3.13)

# Padrões de linguagem
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Configuração da placa e Wi-Fi
set(PICO_BOARD pico_w CACHE STRING "Board type")  # Define a placa como Pico W
set(PICO_CYW43_ARCH_LWIP 1)  # Ativa o suporte ao lwIP para Wi-Fi

# Inclui o SDK do Raspberry Pi Pico
include(pico_sdk_import.cmake)

# Define o projeto
project(atendimento_presencial C CXX ASM)

# Inicializa o SDK do Raspberry Pi Pico
pico_sdk_init()

# Adiciona o executável
add_executable(atendimento_presencial atendimento_presencial.c)

# Configura nome e versão do programa
pico_set_program_name(atendimento_presencial "atendimento_presencial")
pico_set_program_version(atendimento_presencial "0.1")

# Habilita/desabilita saída padrão (UART/USB)
pico_enable_stdio_uart(atendimento_presencial 0)  # Desabilita UART
pico_enable_stdio_usb(atendimento_presencial 1)  # Habilita USB

# Adiciona as bibliotecas necessárias
target_link_libraries(atendimento_presencial
        pico_stdlib                  # Biblioteca padrão do Pico
        hardware_i2c                 # Suporte a I2C
        hardware_gpio                # Suporte a GPIO
        pico_cyw43_arch_lwip_threadsafe_background  # Suporte ao Wi-Fi com lwIP
        pico_lwip                    # Biblioteca lwIP para redes
        )

# Adiciona diretórios de inclusão necessários
target_include_directories(atendimento_presencial PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}    # Diretório atual do projeto
        ${PICO_SDK_PATH}/src/rp2_common/hardware_i2c/include  # Headers para I2C
        ${PICO_SDK_PATH}/src/rp2_common/hardware_gpio/include # Headers para GPIO
        ${PICO_SDK_PATH}/src/rp2_common/pico_cyw43_arch/include  # Headers para Wi-Fi
        ${PICO_SDK_PATH}/lib/cyw43-driver/src  # Headers para o driver CYW43
        ${PICO_SDK_PATH}/lib/lwip/src/include  # Headers para lwIP
        )

# Gera arquivos extras para o build (como .uf2 e .hex)
pico_add_extra_outputs(atendimento_presencial)