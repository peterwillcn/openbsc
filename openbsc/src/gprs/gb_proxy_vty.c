/*
 * (C) 2010 by Harald Welte <laforge@gnumonks.org>
 * (C) 2011 by Holger Hans Peter Freyther
 * (C) 2010-2011 by On-Waves
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <osmocom/core/talloc.h>

#include <openbsc/debug.h>
#include <openbsc/gb_proxy.h>
#include <openbsc/gprs_ns.h>
#include <openbsc/vty.h>

#include <osmocom/vty/command.h>
#include <osmocom/vty/vty.h>

static struct gbproxy_config *g_cfg = NULL;

/*
 * vty code for mgcp below
 */
static struct cmd_node gbproxy_node = {
	GBPROXY_NODE,
	"%s(gbproxy)#",
	1,
};

extern int dl_skip_ms_ra;
extern int dl_skip_prio;
extern int dl_skip_imsi;
extern int dl_skip_old_tlli;
extern int dl_skip_flow_id;
extern int dl_skip_lsa;
extern int dl_skip_utran;
extern int dl_max_pdu_time;

static int config_write_gbproxy(struct vty *vty)
{
	vty_out(vty, "gbproxy%s", VTY_NEWLINE);

	vty_out(vty, " sgsn nsei %u%s", g_cfg->nsip_sgsn_nsei,
		VTY_NEWLINE);
	vty_out(vty, " dl-pdu-max-lifetime %d%s",
		dl_max_pdu_time, VTY_NEWLINE);
	vty_out(vty, " dl-skip ms-ra %d%s", dl_skip_ms_ra, VTY_NEWLINE);
	vty_out(vty, " dl-skip priority %d%s", dl_skip_prio, VTY_NEWLINE);
	vty_out(vty, " dl-skip imsi %d%s", dl_skip_imsi, VTY_NEWLINE);
	vty_out(vty, " dl-skip old-tlli %d%s", dl_skip_old_tlli, VTY_NEWLINE);
	vty_out(vty, " dl-skip packet-flow-id %d%s",
		dl_skip_flow_id, VTY_NEWLINE);
	vty_out(vty, " dl-skip lsa %d%s", dl_skip_lsa, VTY_NEWLINE);
	vty_out(vty, " dl-skip utran %d%s", dl_skip_utran, VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN(cfg_gbproxy,
      cfg_gbproxy_cmd,
      "gbproxy",
      "Configure the Gb proxy")
{
	vty->node = GBPROXY_NODE;
	return CMD_SUCCESS;
}

DEFUN(cfg_nsip_sgsn_nsei,
      cfg_nsip_sgsn_nsei_cmd,
      "sgsn nsei <0-65534>",
      "Set the NSEI to be used in the connection with the SGSN")
{
	unsigned int port = atoi(argv[0]);

	g_cfg->nsip_sgsn_nsei = port;
	return CMD_SUCCESS;
}

#define SKIP_CMD(varname, par) 					\
	DEFUN(varname ## _var, varname ## _cmd,			\
        "dl-skip " par " (0|1)",				\
	"Skip IEs\n" par  "\n" "Keep\n" "Skip\n")		\
{								\
	varname = atoi(argv[0]);				\
	return CMD_SUCCESS;					\
}

SKIP_CMD(dl_skip_ms_ra, "ms-ra")
SKIP_CMD(dl_skip_prio, "priority")
SKIP_CMD(dl_skip_imsi, "imsi")
SKIP_CMD(dl_skip_old_tlli, "old-tlli")
SKIP_CMD(dl_skip_flow_id, "packet-flow-id")
SKIP_CMD(dl_skip_lsa, "lsa")
SKIP_CMD(dl_skip_utran, "utran")


DEFUN(dl_max_pdu_time_func, dl_max_pdu_time_cmd,
      "dl-pdu-max-lifetime <0-65535>",
      "Clamp max PDU time to\n" "Max allowed value\n")
{
	dl_max_pdu_time = atoi(argv[0]);
	return CMD_SUCCESS;
}

int gbproxy_vty_init(void)
{
	install_element_ve(&show_gbproxy_cmd);

	install_element(CONFIG_NODE, &cfg_gbproxy_cmd);
	install_node(&gbproxy_node, config_write_gbproxy);
	install_default(GBPROXY_NODE);
	install_element(GBPROXY_NODE, &ournode_exit_cmd);
	install_element(GBPROXY_NODE, &ournode_end_cmd);
	install_element(GBPROXY_NODE, &cfg_nsip_sgsn_nsei_cmd);

	install_element(GBPROXY_NODE, &dl_skip_ms_ra_cmd);
	install_element(GBPROXY_NODE, &dl_skip_prio_cmd);
	install_element(GBPROXY_NODE, &dl_skip_imsi_cmd);
	install_element(GBPROXY_NODE, &dl_skip_old_tlli_cmd);
	install_element(GBPROXY_NODE, &dl_skip_flow_id_cmd);
	install_element(GBPROXY_NODE, &dl_skip_lsa_cmd);
	install_element(GBPROXY_NODE, &dl_skip_utran_cmd);
	install_element(GBPROXY_NODE, &dl_max_pdu_time_cmd);

	return 0;
}

int gbproxy_parse_config(const char *config_file, struct gbproxy_config *cfg)
{
	int rc;

	g_cfg = cfg;
	rc = vty_read_config_file(config_file, NULL);
	if (rc < 0) {
		fprintf(stderr, "Failed to parse the config file: '%s'\n", config_file);
		return rc;
	}

	return 0;
}

