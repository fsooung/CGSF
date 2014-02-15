// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
///////////////////////////////////////////////////////////////////////////////////
#include <ace/ace.h>
#include <ace/Thread_Manager.h>
#include <ace/Thread_Mutex.h>
#include <ace/Proactor.h>
#include <ace/Task.h>
///////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "SFMacro.h"
#include "SFConstant.h"
#include "SFStructure.h"
#include "Macro.h"

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

using namespace google;

#pragma comment(lib, "enginelayer.lib")
#pragma comment(lib, "baselayer.lib")
#pragma comment(lib, "databaselayer.lib")
#pragma comment(lib, "libprotobuf.lib")

#ifdef _DEBUG
#pragma comment(lib, "aced.lib")
#else
#pragma comment(lib, "ace.lib")
#endif