if __name__ != '__main__':
  raise Exception('Do not import this script')

import sys
import uuid

if __name__ == '__main__':
  generated_uuid = str(uuid.uuid4())
  with open(sys.argv[1], "w") as f:
    f.write(generated_uuid)
  print(f"Generated uuid: {generated_uuid}")
  try:
    if sys.argv[2] == "TARGET":
      print("BUILD *********************")
  except:
    pass
else:
  raise Exception('Do not import this script')
