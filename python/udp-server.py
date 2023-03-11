import socket
import time

# Crea un socket UDP
broadcast_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Establece la opción de socket para permitir la transmisión por broadcast
broadcast_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

# Define la dirección IP y el puerto de destino
broadcast_address = ('<broadcast>', 8000)

# Envía el mensaje por broadcast
# message = "Este es un mensaje de prueba por broadcast"
data = 'x'
while(data != 'hi sup'):

    message = "hi there"
    broadcast_socket.sendto(message.encode(), broadcast_address)

    print(f"Mensaje enviado por broadcast a {broadcast_address}")
    data, server = broadcast_socket.recvfrom(4096)
    print(f'Respuesta recibida de {server}: {data}')
    data = data.decode('utf-8')
    print(data)
    time.sleep(1)
broadcast_socket.close()
