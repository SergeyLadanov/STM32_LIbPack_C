# Вспомогательные переменные
# Имя компилятора
CC = gcc
# Директория с выходными файлами
bin_dir = Bin
# Имя исполняемого файла
app_name = converter


# Пути с исходниками
DIR1 = CP1251_Converter

# Формирование массива каталогов проекта
path_list = $(DIR1)
header_list = $(foreach d, $(path_list), -I $d/Inc)

# Список объектных файлов
objects = $(bin_dir)/main.o $(bin_dir)/cp1251_converter.o

$(shell mkdir -p $(bin_dir))

# Правило для сборки исполняемого файла
app:	$(objects)
	$(CC) $(objects) -o $(bin_dir)/$(app_name)

# Правило сборки основного
$(bin_dir)/%.o: %.c
	$(CC) -Wall -c $< $(header_list) -o $@

# Правило сборки для каталога DIR1
$(bin_dir)/%.o: $(DIR1)/Src/%.c $(wildcard $(DIR1)/Inc/*.h)
	$(CC) -Wall -c $< $(header_list) -o $@


# Правило для очистки
clean:
	rm -r $(bin_dir)

