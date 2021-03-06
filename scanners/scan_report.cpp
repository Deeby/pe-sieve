#include "scan_report.h"

#include "hollowing_scanner.h"
#include "hook_scanner.h"
#include "mempage_scanner.h"
#include "mapping_scanner.h"

void ProcessScanReport::appendToType(ModuleScanReport *report)
{
	if (report == nullptr) return;

	if (dynamic_cast<HeadersScanReport*>(report)) {
		this->reports_by_type[REPORT_HEADERS_SCAN].insert(report);
		return;
	}
	if (dynamic_cast<MemPageScanReport*>(report)) {
		this->reports_by_type[REPORT_MEMPAGE_SCAN].insert(report);
		return;
	}
	if (dynamic_cast<MappingScanReport*>(report)) {
		this->reports_by_type[REPORT_MAPPING_SCAN].insert(report);
		return;
	}
	if (dynamic_cast<CodeScanReport*>(report)) {
		this->reports_by_type[REPORT_CODE_SCAN].insert(report);
		return;
	}
	if (dynamic_cast<UnreachableModuleReport*>(report)) {
		this->reports_by_type[REPORT_UNREACHABLE_SCAN].insert(report);
		return;
	}
	if (dynamic_cast<SkippedModuleReport*>(report)) {
		this->reports_by_type[REPORT_SKIPPED_SCAN].insert(report);
		return;
	}
}

size_t ProcessScanReport::countSuspiciousPerType(report_type_t type) const
{
	if (type >= REPORT_TYPES_COUNT) {
		return 0; //invalid type
	}
	size_t suspicious = 0;
	std::set<ModuleScanReport*>::iterator itr;
	for (itr = this->reports_by_type[type].begin(); itr != this->reports_by_type[type].end(); itr++) {
		ModuleScanReport* report = *itr;
		if (ModuleScanReport::get_scan_status(report) == SCAN_SUSPICIOUS) {
			suspicious++;
		}
	}
	return suspicious;
}

t_report ProcessScanReport::generateSummary() const
{
	t_report summary = { 0 };
	summary.pid = this->pid;
	summary.errors = static_cast<DWORD>(this->errorsCount);
	summary.skipped = static_cast<DWORD>(this->reports_by_type[REPORT_SKIPPED_SCAN].size());
	summary.scanned = static_cast<DWORD>(this->reports_by_type[REPORT_HEADERS_SCAN].size());

	std::vector<ModuleScanReport*>::const_iterator itr = module_reports.begin();
	for (; itr != module_reports.end(); itr++) {
		ModuleScanReport* report = *itr;
		if (ModuleScanReport::get_scan_status(report) == SCAN_SUSPICIOUS) {
			summary.suspicious++;
		}
		if (ModuleScanReport::get_scan_status(report) == SCAN_ERROR) {
			summary.errors++;
		}
		
	}
	summary.hooked = countSuspiciousPerType(REPORT_CODE_SCAN);
	summary.implanted = countSuspiciousPerType(REPORT_MEMPAGE_SCAN);
	summary.replaced = countSuspiciousPerType(REPORT_HEADERS_SCAN);
	summary.detached = countSuspiciousPerType(REPORT_UNREACHABLE_SCAN);
	
	return summary;
}

