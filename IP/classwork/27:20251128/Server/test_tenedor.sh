#!/bin/bash
# Script de pruebas para sistema con Tenedor
# CI-0123 Grupo 6

TENEDOR_IP="127.0.0.1"
TENEDOR_PORT=8080
TEST_DIR="test_results"

# Colores para output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "========================================="
echo "   PRUEBAS DEL SISTEMA CON TENEDOR"
echo "========================================="

# Crear directorio para resultados
mkdir -p $TEST_DIR

# Función para enviar comando
send_command() {
  local command=$1
  local test_name=$2
  echo -e "${YELLOW}[TEST]${NC} $test_name"
  echo -e "$command" | nc $TENEDOR_IP $TENEDOR_PORT > "$TEST_DIR/$test_name.txt"
  
  if [ $? -eq 0 ]; then
    echo -e "${GREEN}[OK]${NC} Respuesta recibida"
    cat "$TEST_DIR/$test_name.txt"
  else
    echo -e "${RED}[FAIL]${NC} Error en comunicación"
  fi
  echo ""
}
      echo -e "${GREEN}[OK]${NC} Respuesta recibida"
      cat "$TEST_DIR/$test_name.txt"
# Test 1: LIST - Listar figuras iniciales
echo -e "\n${YELLOW}=== TEST 1: LIST ===${NC}"
send_command "LIST /BEGIN/ /END/" "test1_list"

# Test 2: GET - Obtener figura existente
echo -e "\n${YELLOW}=== TEST 2: GET gato.dat ===${NC}"
send_command "GET /BEGIN/ gato.dat /END/" "test2_get_gato"

# Test 3: ADD - Agregar nueva figura
echo -e "\n${YELLOW}=== TEST 3: ADD nueva figura ===${NC}"
CAT_CONTENT="  /\_/\  
 ( o.o ) 
  > ^ <"
send_command "ADD /BEGIN/ perro.dat / 50 / $CAT_CONTENT /END/" "test3_add_perro"

# Test 4: LIST - Verificar que se agregó
echo -e "\n${YELLOW}=== TEST 4: LIST (verificar ADD) ===${NC}"
send_command "LIST /BEGIN/ /END/" "test4_list_after_add"

# Test 5: GET - Obtener la nueva figura
echo -e "\n${YELLOW}=== TEST 5: GET perro.dat ===${NC}"
send_command "GET /BEGIN/ perro.dat /END/" "test5_get_perro"

# Test 6: DELETE - Eliminar figura
echo -e "\n${YELLOW}=== TEST 6: DELETE perro.dat ===${NC}"
send_command "DELETE /BEGIN/ perro.dat /END/" "test6_delete_perro"

# Test 7: LIST - Verificar eliminación
echo -e "\n${YELLOW}=== TEST 7: LIST (verificar DELETE) ===${NC}"
send_command "LIST /BEGIN/ /END/" "test7_list_after_delete"

# Test 8: GET - Intentar obtener figura eliminada (debe fallar)
echo -e "\n${YELLOW}=== TEST 8: GET figura eliminada (debe fallar) ===${NC}"
send_command "GET /BEGIN/ perro.dat /END/" "test8_get_deleted"

# Test 9: Comando inválido
echo -e "\n${YELLOW}=== TEST 9: Comando inválido ===${NC}"
send_command "INVALID /BEGIN/ test /END/" "test9_invalid_command"

# Test 10: Formato incorrecto
echo -e "\n${YELLOW}=== TEST 10: Formato incorrecto ===${NC}"
send_command "LIST /BEGIN/" "test10_bad_format"

echo -e "\n========================================="
echo -e "${GREEN}PRUEBAS COMPLETADAS${NC}"
echo "Resultados guardados en: $TEST_DIR/"
echo "========================================="

# Resumen
echo -e "\n${YELLOW}RESUMEN:${NC}"
echo "- Revisa los archivos en $TEST_DIR/ para ver respuestas detalladas"
echo "- Verifica códigos de respuesta (200, 400, 500)"
echo "- Comprueba que las figuras se distribuyen correctamente"

# Prueba de múltiples servidores (si están corriendo)
if pgrep -f "servidor.*8083" > /dev/null; then
  echo -e "\n${YELLOW}=== PRUEBA DE DISTRIBUCIÓN ===${NC}"
  echo "Agregando múltiples figuras para probar distribución..."
  
  for i in {1..5}; do
    send_command "ADD /BEGIN/ figura$i.dat / 20 / Contenido $i /END/" "test_dist_$i"
    sleep 0.5
  done
  
  echo -e "${GREEN}Verifica en los logs de cada servidor qué figuras recibieron${NC}"
fi