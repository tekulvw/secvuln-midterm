
HRESULT GetServerPath(WCHAR *pwszPath, WCHAR **pwszServerPath ) {
   WCHAR *pwszFinalPath = pwszPath;
   WCHAR wszMachineName[MAX_COMPUTERNAME_LENGTH_FQDN+1];
   hr = GetMachineName(pwszPath, wszMachineName);
   *pwszServerPath = pwszFinalPath;
}
 
error_status_t _RemoteActivation(/* ... ,*/ WCHAR *pwszObjectName /*, ...*/ ) {
   *phr = GetServerPath(pwszObjectName, &pwszObjectName);
   /* ... */
}
 
HRESULT GetMachineName(WCHAR *pwszPath,
		       WCHAR wszMachineName[MAX_COMPUTERNAME_LENGTH_FQDN+1]) {
   pwszServerName = wszMachineName;
   LPWSTR pwszTemp = pwszPath + 2;
   while (*pwszTemp != L'\\') *pwszServerName++ = *pwszTemp++;
   /* ... */
}

/*
    This noncompliant code example shows the flawed logic in the Windows
    Distributed Component Object Model (DCOM) Remote Procedure Call (RPC)
    interface that was exploited by the W32.Blaster.Worm. The error is that the
    while loop in the GetMachineName() function (used to extract the host name
    from a longer string) is not sufficiently bounded. When the character array
    pointed to by pwszTemp does not contain the backslash character among the
    first MAX_COMPUTERNAME_LENGTH_FQDN + 1 elements, the final valid iteration
    of the loop will dereference past the end pointer, resulting in exploitable
    undefined behavior 47. In this case, the actual exploit allowed the attacker
    to inject executable code into a running program. Economic damage from the
    Blaster worm has been estimated to be at least $525 million [Pethia 2003].

    For a discussion of this programming error in the Common Weakness
    Enumeration database, see CWE-119, "Improper Restriction of Operations
    within the Bounds of a Memory Buffer," and CWE-121, "Stack-based Buffer
    Overflow"
*/
