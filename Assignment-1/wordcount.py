import sys
fnames = ["Assgn1_1_18.sh", "Assgn1_2_18.sh",
          "Assgn1_4_18.sh", "Assgn1_7_18.sh", "Assgn1_9_18.sh"]
total_count = 0
for fname in fnames:
    in_file = open(fname)
    content = in_file.read().replace('=', ' = ').replace('|', ' | ').replace(';', ' ; ').replace(',', ' , ').replace('<', ' < ').replace('>', ' > ').replace('(', ' ( ').replace(')', ' ) ').replace('[', ' [ ').replace(']', ' ] ').replace('{', ' { ').replace('}', ' } ')
    word_count = len(content.split())
    total_count += word_count
    print(f'Word count of {fname}: {word_count}')
print(f"Total word count: {total_count}")

