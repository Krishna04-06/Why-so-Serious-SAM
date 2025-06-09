

# Windows Forensics + HiveNightmare Threat Detection

## 1. Investigation Workflow

1. Acquisition  
2. Live Scanning  
3. Memory Forensics  
4. Disk Mount & Triage  
5. Persistence Analysis  
6. Malware Analysis  
7. System Registry Audit  
8. Root Cause Investigation  
9. Lateral Movement Tracking  
10. Timeline Construction  
11. USB and MountPoint History  
12. Data Recovery (Deleted/VSS)  
13. Malware RE  
14. Threat-Specific Detection 
15. Reporting

---

## 2. Tooling

- **Disk & Memory Acquisition**: FTK Imager, Magnet RAM Capturer, KAPE  
- **Memory Analysis**: Volatility, MemProcFS  
- **Disk Analysis**: Autopsy, Arsenal Image Mounter, EZTools  
- **Log & Timeline**: Event Log Explorer, Chainsaw, Hayabusa, DeepBlueCLI  
- **Live Triage**: Inquisitor, Redline, Velociraptor

---

## 3. HiveNightmare (CVE‑2021‑36934) Detection

### TTP Summary

- **Exploit Name**: HiveNightmare / SeriousSAM  
- **CVE**: CVE-2021-36934  
- **Attack Vector**: Misconfigured access to registry hive backups via Volume Shadow Copy (VSS)  
- **MITRE ATT&CK TTPs**:
  - T1003.002 – OS Credential Dumping: SAM
  - T1087.002 – Account Discovery
  - T1056.001 – Input Capture (Persistence)
  - T1003.006 – Credential Dumping via VSS

---

## Detection Events

| Event ID | Description | Logic |
|----------|-------------|-------|
| 4688 | Process creation | Non-admin runs `HiveNightmare.exe`, `vssadmin`, or `diskshadow` |
| 4656 | Object handle request | Access attempt to `SAM`, `SYSTEM`, or `SECURITY` hives |
| 4663 | Object access | Read/Write to hive files |
| 8003 | VSS snapshot created (optional audit policy) | Shadow copy abuse |

---

## Detection Queries

**Suspicious Process Creation (KQL/Elastic):**

```kql
EventID == 4688 AND (
  NewProcessName CONTAINS "vssadmin.exe" OR
  NewProcessName CONTAINS "diskshadow.exe" OR
  CommandLine CONTAINS "shadow" OR "HiveNightmare"
) AND
SubjectUserGroup != "Administrators"
