// version 0.1.0
/*
 * Only for ESP32
 * ESP8266 is not work
 */
static String mac_address_str = "";

void print_mac(const unsigned char *mac)
{
  printf("\n\t%02X:%02X:%02X:%02X:%02X:%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  printf("\n\t%d:%d:%d:%d:%d:%d\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}

void PrintMacAddress()
{
  unsigned char mac_base[6] = {0};
  unsigned char mac_local_base[6] = {0};
  unsigned char mac_uni_base[6] = {0};

  esp_efuse_mac_get_default(mac_base);
  esp_read_mac(mac_base, ESP_MAC_WIFI_STA);
  esp_derive_local_mac(mac_local_base, mac_uni_base);
  
  printf("Local Address: ");
  print_mac(mac_local_base);
  
  printf("Uni Address: ");
  print_mac(mac_uni_base);
  
  printf("MAC Address: ");
  print_mac(mac_base);
}

String GetMacAddressString()
{
  if (!mac_address_str.equals(""))
    return mac_address_str;
  
  uint8_t mac_address[6];
  esp_efuse_mac_get_default(mac_address);
  mac_address_str = String(mac_address[0]);
  for(int i = 1; i < 6; i++)
  {
    mac_address_str = String(mac_address_str + ":");
    mac_address_str = String(mac_address_str + mac_address[i]);
  }
  return mac_address_str;
}
