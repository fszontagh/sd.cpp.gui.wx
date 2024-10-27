import argparse
import time

import polib
import requests


API = ""

def translate_text(prompt, model):

    url = API + "/api/generate"
    payload = {
        "model": model,
        "prompt": prompt,
        "stream": False
    }


    response = requests.post(url, json=payload)
    if response.status_code == 200:
        data = response.json()
        translated_text = data.get("response")
        return translated_text
    else:
        print("Failed to translate text. Status code:", response.status_code)
        return None

def translate_text_with_retry(prompt, model, max_retries=3):
    for _ in range(max_retries):
        response = translate_text(prompt, model)
        if response:
            return response
        else:
            print("Retrying translation...")
            time.sleep(1)  # 等待一秒钟后重试
    print("Failed to translate text after", max_retries, "attempts. Keeping original value.")
    return prompt



def main():
    # 解析命令行参数
    parser = argparse.ArgumentParser(description="Translate gettext PO file using OpenAI API")
    parser.add_argument("input_file", help="Input gettext PO file path")
    parser.add_argument("output_file", help="Output translated gettext PO file path")
    parser.add_argument("model", help="Model for translation")
    parser.add_argument("--api", default="http://127.0.0.1:11434", help="ollama API url")
    parser.add_argument("--debug", default=False,action="store_true", help="debug print translated")
    parser.add_argument("--prompt", default="You are a professional document translator who is proficient in multiple languages. Please translate the following English part into German, and keep the RST content, without translating special symbols or punctuation marks."
                                                    "Do not translate \":ref:\" Only output the result:\n", help="translation prompt")
    args = parser.parse_args()
    global API
    API = args.api
    prompt = args.prompt
    debug = args.debug

    # 读取输入的 PO 文件
    input_po = polib.pofile(args.input_file)
    from progress.bar import IncrementalBar
    bar = IncrementalBar('Transitioning', max=len(input_po))
    # 遍历 PO 文件中的条目进行翻译
    for entry in input_po:
        # 提取文本内容
        bar.next()
        text = entry.msgstr or entry.msgid

        # 忽略 rst 中的特殊标记
        
        if ":ref:" in text:

            bar.finish()
            continue

        # 翻译文本内容
        translated_text = translate_text_with_retry(prompt+text, args.model)
        if debug:
            print("\nOriginal:", text)
            print("Translated:", translated_text)
        # 更新 PO 条目的翻译结果
        if translated_text:
            entry.msgstr = translated_text
        bar.finish()

    # 将翻译后的 PO 文件写入输出文件
    input_po.save(args.output_file)

if __name__ == "__main__":
    main()
