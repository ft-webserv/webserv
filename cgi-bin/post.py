#!/usr/bin/env python3
import sys
import os

# 표준 출력 스트림의 텍스트 인코딩 설정
sys.stdout.reconfigure(encoding='utf-8')

# 헤더 출력
print("Content-Type: text/plain;charset=utf-8")
print()

# 표준 입력에서 데이터 읽기
data = sys.stdin.read()

# 입력된 데이터가 없을 경우 에러 메시지 출력
if not data:
    print("No data received.")
    sys.exit(1)

# 대문자로 변환하여 출력
result = data.upper()
print(result)
