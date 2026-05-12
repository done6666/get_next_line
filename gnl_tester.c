/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl_tester.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opektas <opektas@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 00:00:00 by opektas           #+#    #+#             */
/*   Updated: 2026/05/12 17:54:43 by opektas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ===== RENK TANIMLARI ===== */
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

/* ===== SAYAÇLAR ===== */
static int	g_pass = 0;
static int	g_fail = 0;

/* ===== YARDIMCI FONKSİYONLAR ===== */

static void	print_header(const char *title)
{
	printf("\n" BOLD CYAN "══════════════════════════════════════\n");
	printf("  %s\n", title);
	printf("══════════════════════════════════════\n" RESET);
}

static void	print_result(const char *test_name, int passed, const char *detail)
{
	if (passed)
	{
		printf(GREEN "  [PASS]" RESET " %s\n", test_name);
		g_pass++;
	}
	else
	{
		printf(RED "  [FAIL]" RESET " %s", test_name);
		if (detail)
			printf(" → %s", detail);
		printf("\n");
		g_fail++;
	}
}

/* Satırın '\n' ile bitip bitmediğini kontrol eder */
static int	ends_with_newline(const char *line)
{
	size_t	len;

	if (!line)
		return (0);
	len = strlen(line);
	return (len > 0 && line[len - 1] == '\n');
}

/* Dosyayı okuyup kaç satır olduğunu sayar (ground truth) */
static int	count_lines_in_file(const char *path)
{
	FILE	*f;
	int		count;
	int		c;
	int		last;

	f = fopen(path, "r");
	if (!f)
		return (-1);
	count = 0;
	last = 0;
	while ((c = fgetc(f)) != EOF)
	{
		last = c;
		if (c == '\n')
			count++;
	}
	/* Newline ile bitmeyen dosyanın son satırını say */
	if (last != '\n' && last != 0)
		count++;
	fclose(f);
	return (count);
}

/* ===== TEST FONKSİYONLARI ===== */

/*
** TEST 1: Normal dosya
** Satır sayısı doğru mu? Her satır '\n' ile bitiyor mu?
*/
static void	test_normal_file(void)
{
	int		fd;
	char	*line;
	int		line_count;
	int		expected;
	char	detail[128];

	print_header("TEST 1: Normal Dosya");
	fd = open("test_normal.txt", O_RDONLY);
	expected = count_lines_in_file("test_normal.txt");
	line_count = 0;
	while ((line = get_next_line(fd)))
	{
		line_count++;
		snprintf(detail, sizeof(detail), "satir %d newline ile bitmiyor", line_count);
		print_result("Satir newline ile bitiyor", ends_with_newline(line), detail);
		free(line);
	}
	snprintf(detail, sizeof(detail), "beklenen %d, alinan %d", expected, line_count);
	print_result("Satir sayisi dogru", line_count == expected, detail);
	close(fd);
}

/*
** TEST 2: Boş dosya
** İlk çağrıda NULL dönmeli
*/
static void	test_empty_file(void)
{
	int		fd;
	char	*line;

	print_header("TEST 2: Bos Dosya");
	fd = open("test_empty.txt", O_RDONLY);
	line = get_next_line(fd);
	print_result("Bos dosyada NULL donuyor", line == NULL, NULL);
	if (line)
		free(line);
	close(fd);
}

/*
** TEST 3: Newline'sız son satır
** Son satır '\n' olmadan da okunabilmeli
*/
static void	test_no_newline_at_end(void)
{
	int		fd;
	char	*line;
	int		line_count;
	int		expected;
	char	detail[128];

	print_header("TEST 3: Sonda Newline Yok");
	fd = open("test_no_newline.txt", O_RDONLY);
	expected = count_lines_in_file("test_no_newline.txt");
	line_count = 0;
	while ((line = get_next_line(fd)))
	{
		line_count++;
		free(line);
	}
	snprintf(detail, sizeof(detail), "beklenen %d, alinan %d", expected, line_count);
	print_result("Son satir newline olmadan okunuyor", line_count == expected, detail);
	close(fd);
}

/*
** TEST 4: Geçersiz fd
** Negatif fd'de NULL dönmeli
*/
static void	test_invalid_fd(void)
{
	char	*line;

	print_header("TEST 4: Gecersiz FD");
	line = get_next_line(-1);
	print_result("fd=-1 icin NULL donuyor", line == NULL, NULL);
	if (line)
		free(line);
	line = get_next_line(99999);
	print_result("fd=99999 icin NULL donuyor", line == NULL, NULL);
	if (line)
		free(line);
}

/*
** TEST 5: Tek '\n' satırlar
** Sadece newline içeren dosya
*/
static void	test_only_newlines(void)
{
	int		fd;
	char	*line;
	int		line_count;
	int		expected;
	char	detail[128];

	print_header("TEST 5: Sadece Newline");
	fd = open("test_only_newlines.txt", O_RDONLY);
	expected = count_lines_in_file("test_only_newlines.txt");
	line_count = 0;
	while ((line = get_next_line(fd)))
	{
		line_count++;
		snprintf(detail, sizeof(detail), "satir %d yanlis: '%s'", line_count, line);
		print_result("Satir sadece newline iceriyor",
			strcmp(line, "\n") == 0, detail);
		free(line);
	}
	snprintf(detail, sizeof(detail), "beklenen %d, alinan %d", expected, line_count);
	print_result("Satir sayisi dogru", line_count == expected, detail);
	close(fd);
}

/*
** TEST 6: Uzun satır (10000 karakter)
** BUFFER_SIZE ne olursa olsun doğru okunmalı
*/
static void	test_long_line(void)
{
	int		fd;
	char	*line;
	size_t	len;
	char	detail[128];

	print_header("TEST 6: Uzun Satir (10000 karakter)");
	fd = open("test_long_line.txt", O_RDONLY);
	line = get_next_line(fd);
	if (!line)
	{
		print_result("Uzun satir okunuyor", 0, "NULL dondu");
		close(fd);
		return ;
	}
	len = strlen(line);
	/* '\n' dahil 10001 karakter bekliyoruz */
	snprintf(detail, sizeof(detail), "beklenen 10001, alinan %zu", len);
	print_result("Uzun satir uzunlugu dogru", len == 10001, detail);
	print_result("Uzun satir newline ile bitiyor", ends_with_newline(line), NULL);
	free(line);
	/* Bir sonraki cagri NULL donmeli */
	line = get_next_line(fd);
	print_result("Uzun satirdan sonra NULL donuyor", line == NULL, NULL);
	if (line)
		free(line);
	close(fd);
}

/*
** TEST 7: Tekrar eden get_next_line çağrısı
** Dosya bittikten sonra defalarca NULL dönmeli
*/
static void	test_repeated_null(void)
{
	int		fd;
	char	*line;
	int		passed;
	int		i;

	print_header("TEST 7: Dosya Bittikten Sonra Tekrar Cagri");
	fd = open("test_normal.txt", O_RDONLY);
	while ((line = get_next_line(fd)))
		free(line);
	passed = 1;
	i = 0;
	while (i < 5)
	{
		line = get_next_line(fd);
		if (line != NULL)
			passed = 0;
		if (line)
			free(line);
		i++;
	}
	print_result("Dosya bittikten sonra NULL doniyor (5 kez)", passed, NULL);
	close(fd);
}

/*
** TEST 8: İçerik doğruluğu
** Okunan satırlar gerçekten doğru içeriğe sahip mi?
*/
static void	test_content_correctness(void)
{
	int			fd;
	char		*line;
	const char	*expected[] = {"satir 1\n", "satir 2\n", "satir 3\n", NULL};
	int			i;
	char		detail[256];

	print_header("TEST 8: Icerik Dogrulugu");
	fd = open("test_normal.txt", O_RDONLY);
	i = 0;
	while ((line = get_next_line(fd)))
	{
		if (expected[i])
		{
			snprintf(detail, sizeof(detail),
				"beklenen '%s', alinan '%s'", expected[i], line);
			print_result("Satir icerigi dogru",
				strcmp(line, expected[i]) == 0, detail);
		}
		free(line);
		i++;
	}
	close(fd);
}

/*
** TEST 9: Karışık boş satırlar
** Boş satırlar arasındaki dolu satırlar kaybolmamalı
*/
static void	test_mixed_empty_lines(void)
{
	int		fd;
	char	*line;
	int		line_count;
	int		expected;
	char	detail[128];

	print_header("TEST 9: Karisik Bos Satirlar");
	fd = open("test_mixed.txt", O_RDONLY);
	expected = count_lines_in_file("test_mixed.txt");
	line_count = 0;
	while ((line = get_next_line(fd)))
	{
		line_count++;
		free(line);
	}
	snprintf(detail, sizeof(detail), "beklenen %d, alinan %d", expected, line_count);
	print_result("Karisik dosyada satir sayisi dogru",
		line_count == expected, detail);
	close(fd);
}

/*
** TEST 10: Tek karakter satırlar
*/
static void	test_single_char_lines(void)
{
	int		fd;
	char	*line;
	int		line_count;
	int		expected;
	char	detail[128];

	print_header("TEST 10: Tek Karakter Satirlar");
	fd = open("test_single_chars.txt", O_RDONLY);
	expected = count_lines_in_file("test_single_chars.txt");
	line_count = 0;
	while ((line = get_next_line(fd)))
	{
		snprintf(detail, sizeof(detail),
			"satir uzunlugu 2 olmali, alinan %zu", strlen(line));
		print_result("Tek karakter satir uzunlugu 2 (char+newline)",
			strlen(line) == 2, detail);
		free(line);
		line_count++;
	}
	snprintf(detail, sizeof(detail), "beklenen %d, alinan %d", expected, line_count);
	print_result("Satir sayisi dogru", line_count == expected, detail);
	close(fd);
}

/* ===== ÖZET ===== */
static void	print_summary(void)
{
	int total;

	total = g_pass + g_fail;
	printf("\n" BOLD "══════════════════════════════════════\n");
	printf("  SONUC: %d/%d test gecti\n", g_pass, total);
	if (g_fail == 0)
		printf(GREEN "  Tum testler basarili! \n" RESET);
	else
		printf(RED "  %d test basarisiz!\n" RESET, g_fail);
	printf(BOLD "══════════════════════════════════════\n" RESET "\n");
}

static void	create_test_files(void)
{
	FILE	*f;
	int		i;

	f = fopen("test_normal.txt", "w");
	fprintf(f, "satir 1\nsatir 2\nsatir 3\n");
	fclose(f);

	f = fopen("test_empty.txt", "w");
	fclose(f);

	f = fopen("test_no_newline.txt", "w");
	fprintf(f, "son satir newline yok");
	fclose(f);

	f = fopen("test_only_newlines.txt", "w");
	fprintf(f, "\n\n\n");
	fclose(f);

	f = fopen("test_single_chars.txt", "w");
	fprintf(f, "a\nb\nc\nd\ne\n");
	fclose(f);

	f = fopen("test_mixed.txt", "w");
	fprintf(f, "dolu\n\nbos satirdan sonra\n\n\ndolu\n");
	fclose(f);

	f = fopen("test_long_line.txt", "w");
	i = 0;
	while (i++ < 10000)
		fputc('A', f);
	fputc('\n', f);
	fclose(f);
}

static void	cleanup_test_files(void)
{
	unlink("test_normal.txt");
	unlink("test_empty.txt");
	unlink("test_no_newline.txt");
	unlink("test_only_newlines.txt");
	unlink("test_single_chars.txt");
	unlink("test_mixed.txt");
	unlink("test_long_line.txt");
}
int	main(void)
{
	printf(BOLD "\nGET_NEXT_LINE TESTER\n" RESET);
	printf("BUFFER_SIZE = %d\n", BUFFER_SIZE);
	create_test_files();
	test_normal_file();
	test_empty_file();
	test_no_newline_at_end();
	test_invalid_fd();
	test_only_newlines();
	test_long_line();
	test_repeated_null();
	test_content_correctness();
	test_mixed_empty_lines();
	test_single_char_lines();

	print_summary();
	cleanup_test_files();
	return (g_fail > 0 ? 1 : 0);
}
