import sys
import os
import re

def add_syscall(signature):
    # Регулярное выражение для парсинга сигнатуры типа: int mycall(int a, char* b)
    match = re.match(r'\s*(\w+(?:\s*\*+)?)\s+(\w+)\s*\((.*)\)', signature)
    if not match:
        print("Ошибка: Неверный формат сигнатуры. Используйте например: 'int mycall(int arg1, char* arg2)'")
        return

    ret_type = match.group(1).strip()
    name = match.group(2).strip()
    args = match.group(3).strip()

    paths = {
        "syscall_h": "kernel/syscall.h",
        "syscall_c": "kernel/syscall.c",
        "user_h": "user/user.h",
        "usys_pl": "user/usys.pl"
    }

    for path in paths.values():
        if not os.path.exists(path):
            print(f"Ошибка: Файл {path} не найден.")
            return

    # 1. kernel/syscall.h — Поиск свободного номера
    with open(paths["syscall_h"], "r") as f:
        lines = f.readlines()
    
    max_num = 0
    for line in lines:
        m = re.search(r'#define SYS_(\w+)\s+(\d+)', line)
        if m:
            num = int(m.group(2))
            max_num = max(max_num, num)
    
    new_num = max_num + 1
    with open(paths["syscall_h"], "a") as f:
        f.write(f"#define SYS_{name} {new_num}\n")
    print(f"[+] {paths['syscall_h']}: Добавлен SYS_{name} ({new_num})")

    # 2. kernel/syscall.c — extern и таблица
    with open(paths["syscall_c"], "r") as f:
        content = f.read()

    extern_decl = f"extern uint64 sys_{name}(void);\n"
    if f"sys_{name}" not in content:
        # Вставляем перед последним extern или перед началом таблицы
        content = re.sub(r'(extern uint64 sys_\w+\(void\);(?!\s*extern))', 
                         r'\1\n' + extern_decl, content, count=1)

    # Добавляем в массив обработчиков перед закрывающей скобкой
    entry = f"[SYS_{name}]   sys_{name},\n"
    if f"[SYS_{name}]" not in content:
        content = content.replace("};", f"  {entry}}};")
    
    with open(paths["syscall_c"], "w") as f:
        f.write(content)
    print(f"[+] {paths['syscall_c']}: Обновлена таблица вызовов")

    # 3. user/user.h — Полная сигнатура
    with open(paths["user_h"], "r") as f:
        u_lines = f.readlines()
    
    # Ищем системные вызовы (обычно в начале файла)
    new_line = f"{ret_type} {name}({args});\n"
    inserted = False
    for i, line in enumerate(u_lines):
        if "int main(int, char**);" in line:
            u_lines.insert(i, new_line)
            inserted = True
            break
    
    if not inserted: u_lines.append(new_line)

    with open(paths["user_h"], "w") as f:
        f.writelines(u_lines)
    print(f"[+] {paths['user_h']}: Добавлен прототип '{new_line.strip()}'")

    # 4. user/usys.pl — Точка входа
    with open(paths["usys_pl"], "r") as f:
        if f'entry("{name}")' not in f.read():
            with open(paths["usys_pl"], "a") as f_a:
                f_a.write(f'entry("{name}");\n')
            print(f"[+] {paths['usys_pl']}: Добавлена entry(\"{name}\")")

    print(f"\nУспешно! Теперь реализуйте 'uint64 sys_{name}(void)' в ядре.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Использование: python3 script.py \"int my_call(int, char*)\"")
    else:
        # Собираем все аргументы в одну строку на случай, если пользователь забыл кавычки
        add_syscall(" ".join(sys.argv[1:]))