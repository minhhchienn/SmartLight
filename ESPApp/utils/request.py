import requests

# Địa chỉ IP của ESP32
esp32_ip = "http://192.168.1.155"

# Gửi yêu cầu GET đến endpoint /led1
response = requests.get(f"{esp32_ip}/led")

# Kiểm tra mã trạng thái của phản hồi
if response.status_code == 200:
    # In ra nội dung của phản hồi
    print("Response:", response.text)
else:
    print("Failed to get response from ESP32")