import boto3

s3 = boto3.resource('s3')
bucket_name = "BUCKET_NAME"
file_name = "FILE_NAME"

def read_from_s3():
    obj = s3.Object(bucket_name, file_name)
    body = obj.get()['Body'].read()
    data = body.decode("utf-8") 
    return data

def write_to_s3(data):
    lambda_path = "/tmp/" + file_name
    s3_path = file_name
    s3.Bucket(bucket_name).put_object(Key=s3_path, Body=data)