# Controle de Relé via Rede com ESP-01S

Este projeto apresenta uma solução simples e de baixo custo para controle de cargas via rede local utilizando o módulo **ESP-01S** e o módulo **Relé v4.0**.

## 🎯 Objetivo
Permitir que o relé seja acionado por **500 ms** a partir de um comando HTTP simples — ideal para substituir botões físicos ou automatizar tarefas pontuais.

---

## ⚙️ Como funciona
- Um **servidor Web** escuta em uma porta customizada (ex.: `8081`) e responde a comandos como:
  - `/pulse` → Pulso de 500 ms no relé
  - `/reset` → Reset do módulo
- **Autenticação HTTP básica** para acessos via navegador.
- **MikroTik** pode enviar comandos automáticos sem autenticação.
- **IP fixo** definido no código.
- Possibilidade de uso da biblioteca **WiFiManager** para facilitar a configuração de rede sem necessidade de reprogramar o módulo.

---

## 📦 Lista de Materiais
| Item | Quantidade | Observação |
|------|------------|------------|
| ESP-01S | 1 | Módulo Wi-Fi |
| Módulo Relé v4.0 | 1 | Compatível com ESP-01S |
| Fonte 5V | 1 | Pode ser USB ou fonte dedicada ou conversor Step-Down |
| Cabos jumper | - | Para montagem |

---

## 💻 Código
O código-fonte está disponível na pasta `/src` deste repositório.  
Principais funções:
- `handlePulse()` → Aciona o relé por 500 ms  
- `handleReset()` → Reinicia o módulo  
- `handlePowerOff()` → Mantém o relé ligado por 3 segundos  
- Monitoramento opcional de comunicação com um computador ou servidor



---
