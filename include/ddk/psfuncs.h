/* $Id: psfuncs.h,v 1.9 2000/07/04 08:52:34 dwelch Exp $
 */
#ifndef _INCLUDE_DDK_PSFUNCS_H
#define _INCLUDE_DDK_PSFUNCS_H

PACCESS_TOKEN PsReferenceEffectiveToken(struct _ETHREAD* Thread,
					PTOKEN_TYPE TokenType,
					PUCHAR b,
					PSECURITY_IMPERSONATION_LEVEL Level);

NTSTATUS PsOpenTokenOfProcess(HANDLE ProcessHandle,
			      PACCESS_TOKEN* Token);
NTSTATUS
STDCALL
PsAssignImpersonationToken (
	PETHREAD	Thread,
	HANDLE		TokenHandle
	);

HANDLE STDCALL PsGetCurrentProcessId(VOID);
HANDLE STDCALL PsGetCurrentThreadId(VOID);

/*
 * FUNCTION: Creates a thread which executes in kernel mode
 * ARGUMENTS:
 *       ThreadHandle (OUT) = Caller supplied storage for the returned thread 
 *                            handle
 *       DesiredAccess = Requested access to the thread
 *       ObjectAttributes = Object attributes (optional)
 *       ProcessHandle = Handle of process thread will run in
 *                       NULL to use system process
 *       ClientId (OUT) = Caller supplied storage for the returned client id
 *                        of the thread (optional)
 *       StartRoutine = Entry point for the thread
 *       StartContext = Argument supplied to the thread when it begins
 *                     execution
 * RETURNS: Success or failure status
 */
NTSTATUS STDCALL PsCreateSystemThread(PHANDLE ThreadHandle,
				      ACCESS_MASK DesiredAccess,
				      POBJECT_ATTRIBUTES ObjectAttributes,
				      HANDLE ProcessHandle,
				      PCLIENT_ID ClientId,
				      PKSTART_ROUTINE StartRoutine,
				      PVOID StartContext);
NTSTATUS STDCALL PsTerminateSystemThread(NTSTATUS ExitStatus);
ULONG PsSuspendThread(struct _ETHREAD* Thread,
		      PNTSTATUS WaitStatus,
		      UCHAR Alertable,
		      ULONG WaitMode);
ULONG PsResumeThread(struct _ETHREAD* Thread,
		     PNTSTATUS WaitStatus);
struct _ETHREAD* PsGetCurrentThread(VOID);
struct _EPROCESS* PsGetCurrentProcess(VOID);
PACCESS_TOKEN STDCALL PsReferenceImpersonationToken(struct _ETHREAD* Thread,
						    PULONG Unknown1,
						    PULONG Unknown2,
						    SECURITY_IMPERSONATION_LEVEL* 
						    Level);
PACCESS_TOKEN STDCALL PsReferencePrimaryToken(struct _EPROCESS* Process);
NTSTATUS STDCALL PsAssignImpersonationToken(struct _ETHREAD* Thread,
					    HANDLE TokenHandle);

VOID STDCALL PsImpersonateClient(struct _ETHREAD* Thread,
				 PACCESS_TOKEN Token,
				 UCHAR b,
				 UCHAR c,
				 SECURITY_IMPERSONATION_LEVEL Level);
VOID STDCALL PsRevertToSelf(struct _ETHREAD* Thread);

BOOLEAN STDCALL PsGetVersion (PULONG		MajorVersion	OPTIONAL,
			      PULONG		MinorVersion	OPTIONAL,
			      PULONG		BuildNumber	OPTIONAL,
			      PUNICODE_STRING	CSDVersion	OPTIONAL);

#endif

/* EOF */
